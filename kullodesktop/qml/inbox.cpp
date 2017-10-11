/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "inbox.h"

#include <exception>
#include <QDir>
#include <QFile>
#include <QQmlEngine>
#include <QSettings>
#include <QSet>
#include <QTimer>

#include <apimirror/ClientCreateSessionListener.h>
#include <apimirror/SessionListener.h>
#include <desktoputil/asynctask.h>
#include <desktoputil/databasefiles.h>
#include <desktoputil/kulloversion.h>
#include <desktoputil/qtypestreamers.h>
#include <desktoputil/versions.h>
#include <kulloclient/api/AddressHelpers.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/Client.h>
#include <kulloclient/api/Conversations.h>
#include <kulloclient/api/MasterKeyHelpers.h>
#include <kulloclient/api/Messages.h>
#include <kulloclient/api/NetworkError.h>
#include <kulloclient/api/Syncer.h>
#include <kulloclient/api/SyncMode.h>
#include <kulloclient/api/SyncProgress.h>
#include <kulloclient/api/UserSettings.h>
#include <kulloclient/api_impl/debug.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/util/stltaskrunner.h>

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/qml/conversationlistsorted.h"
#include "kullodesktop/qml/conversationlistsortedfiltered.h"
#include "kullodesktop/qml/conversationlistsource.h"
#include "kullodesktop/qml/innerapplication.h"
#include "kullodesktop/qml/sender.h"
#include "kullodesktop/qml/usersettings.h"

namespace KulloDesktop {
namespace Qml {

Inbox::Inbox(InnerApplication &innerApplication,
             ApiMirror::Client &client,
             Kullo::Util::StlTaskRunner *taskRunner,
             QObject *parent)
    : QObject(parent)
    , innerApplication_(innerApplication)
    , taskRunner_(taskRunner)
    , eventDispatcher_()
    , syncerListener_(std::make_shared<ApiMirror::SyncerListener>())
    , client_(client)
{
    QString activeUser = innerApplication_.deviceSettings()->activeUser();
    if (!activeUser.isEmpty())  // load UserSettings if we have an active user
    {
        Log.d() << "Active user: " << activeUser;
        userSettingsModel_ = UserSettings::loadCredentialsForAddress(eventDispatcher_, activeUser);
    }
    else
    {
        Log.d() << "No active user set.";
    }

    connect(syncerListener_.get(), &ApiMirror::SyncerListener::_progressed,
            this, &Inbox::onSyncProgressed);
    connect(syncerListener_.get(), &ApiMirror::SyncerListener::_finished,
            this, &Inbox::onSyncFinished);
    connect(syncerListener_.get(), &ApiMirror::SyncerListener::_error,
            this, &Inbox::onSyncError);
    connect(syncerListener_.get(), &ApiMirror::SyncerListener::_draftPartTooBig,
            this, &Inbox::draftPartTooBig);

    // Pass stuff to Application to be used by the tray icon
    connect(this, &Inbox::hasSessionChanged,
            &innerApplication_, &InnerApplication::hasSessionChanged);
    connect(this, &Inbox::unreadMessagesCountChanged,
            &innerApplication_, &InnerApplication::unreadMessagesCountChanged);
    connect(this, &Inbox::syncFinished,
            &innerApplication_, &InnerApplication::syncFinished);
}

Inbox::~Inbox()
{
}

bool Inbox::hasSession() const
{
    return session_ != nullptr;
}

ConversationListSorted *Inbox::allConversations()
{
    auto ptr = conversationsAll_.get();
    QQmlEngine::setObjectOwnership(ptr, QQmlEngine::CppOwnership);
    return ptr;
}

ConversationListSortedFiltered *Inbox::visibleConversations()
{
    auto ptr = conversationsVisible_.get();
    QQmlEngine::setObjectOwnership(ptr, QQmlEngine::CppOwnership);
    return ptr;
}

void Inbox::createSession()
{
    if (hasSession()) return;

    auto addressString = userSettingsModel_->address();
    kulloAssert(!addressString.isEmpty());
    auto address = *Kullo::Api::AddressHelpers::create(addressString.toStdString());

    // Set active user
    innerApplication_.deviceSettings()->setActiveUser(addressString);
    innerApplication_.deviceSettings()->setLastActiveUser(addressString);

    auto dbFilePath = innerApplication_.databaseFiles().databaseFilepath(address);
    innerApplication_.databaseFiles().prepareDatabaseFolder(address);
    Log.i() << "Creating session using database file " << dbFilePath << " ...";

    setLocalDatabaseKulloVersion(address, DesktopUtil::KulloVersion("0.0.0"));

    auto sessionListener = Kullo::nn_make_shared<ApiMirror::SessionListener>();
    connect(sessionListener.get(), &ApiMirror::SessionListener::_internalEvent,
            &eventDispatcher_, &ApiMirror::EventDispatcher::onInternalEvent);

    auto listener = Kullo::nn_make_shared<ApiMirror::ClientCreateSessionListener>();
    connect(listener.get(), &ApiMirror::ClientCreateSessionListener::_migrationStarted,
            this, &Inbox::onCreateSessionMigrationStarted);
    connect(listener.get(), &ApiMirror::ClientCreateSessionListener::_finished,
            this, &Inbox::onCreateSessionFinished);
    connect(listener.get(), &ApiMirror::ClientCreateSessionListener::_error,
            this, &Inbox::onCreateSessionError);

    createSessionTask_ = client_.raw()->createSessionAsync(
                userSettingsModel_->rawAddress(),
                userSettingsModel_->rawMasterKey(),
                dbFilePath.toStdString(),
                sessionListener,
                listener
                ).as_nullable();

    if (Applications::KulloApplication::FAKE_LONG_MIGRATION)
    {
        QTimer::singleShot(1000, this, SLOT(onCreateSessionMigrationStarted()));
    }
}

void Inbox::closeSession()
{
    if (!hasSession()) return;

    Log.i() << "Closing session ...";

    session_->syncer()->cancel();

    taskRunner_->wait();

    // Throw away
    // -> list of ConversationModel
    //    -> DraftModel
    //    -> MessageListModel
    //       -> MessageListSource
    //          -> list of MessageModel
    //             -> including Sender
    //             -> AttachmentListModel
    //                -> list of AttachmentModel
    conversationsVisible_ = nullptr;
    conversationsSource_ = nullptr;

    createSessionTask_ = nullptr;

    session_ = nullptr;
    innerApplication_.deviceSettings()->setActiveUser("");
    userSettingsModel_ = nullptr;

    taskRunner_->reset();

    emit hasSessionChanged(false);
}

void Inbox::deleteAccountData(const QString &addressString)
{
    auto address = *Kullo::Api::AddressHelpers::create(addressString.toStdString());

    UserSettings::deleteCredentials(address);
    innerApplication_.databaseFiles().removeDatabase(address);
}

QStringList Inbox::allKnownUsersSorted() const
{
    if (!session_) return QStringList();

    QSet<QString> addressStrings;

    for (const auto convId : session_->conversations()->all())
    {
        for (const auto &address : session_->conversations()->participants(convId))
        {
            addressStrings.insert(QString::fromStdString(address.toString()));
        }
    }

    QStringList out(addressStrings.toList());
    out.sort();
    return out;
}

UserSettings *Inbox::userSettings()
{
    auto out = userSettingsModel_.get();
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

void Inbox::addConversation(QString participants)
{
    std::unordered_set<Kullo::Api::Address> participantsSet;
    foreach (QString address, participants.trimmed().split(','))
    {
        participantsSet.insert(*Kullo::Api::AddressHelpers::create(address.toStdString()));
    }

    if (!participantsSet.empty())
    {
        auto convId = session_->conversations()->get(participantsSet);
        if (convId < 0)
        {
            conversationsSource_->setOpenWhenCreated(participantsSet);
            session_->conversations()->add(participantsSet);
        }
        else
        {
            emit openConversation(convId);
        }
    }
}

void Inbox::removeConversation(Kullo::id_type conversationId)
{
    if (session_)
    {
        session_->conversations()->triggerRemoval(conversationId);
    }
}

bool Inbox::sync()
{
    if (!hasSession()) return false;

    auto syncAlreadyRunning = session_->syncer()->isSyncing();
    session_->syncer()->requestSync(Kullo::Api::SyncMode::Everything);
    if (!syncAlreadyRunning) emit syncStarted();
    return true;
}

void Inbox::clearDatabaseAndStoreCredentials(const QString &addressString, const QString &masterKeyPem)
{
    const auto address = *Kullo::Api::AddressHelpers::create(addressString.toStdString());
    const auto masterKey = *Kullo::Api::MasterKeyHelpers::createFromPem(masterKeyPem.toStdString());

    // Clear old user stuff
    innerApplication_.databaseFiles().removeDatabase(address);

    // Store new credentials
    UserSettings::storeCredentials(address, masterKey);
}

void Inbox::loadCredentials(const QString &addressString)
{
    userSettingsModel_ = UserSettings::loadCredentialsForAddress(eventDispatcher_, addressString);
    emit userSettingsChanged();
}

ApiMirror::Client *Inbox::client() const
{
    QQmlEngine::setObjectOwnership(&client_, QQmlEngine::CppOwnership);
    return &client_;
}

std::shared_ptr<Sender> Inbox::latestSenderForAddress(const Kullo::Api::Address &address) const
{
    if (!session_) return nullptr;

    auto msgId = session_->messages()->latestForSender(address);
    if (msgId < 0) return nullptr;
    return std::make_shared<Sender>(session_, msgId);
}

std::shared_ptr<Kullo::Api::Session> Inbox::session() const
{
    return session_;
}

ConversationListSource *Inbox::conversationsListSource()
{
    return conversationsSource_.get();
}

void Inbox::onCreateSessionMigrationStarted()
{
    Log.d() << "Migration started";
    emit migrationStarted();
}

void Inbox::onCreateSessionFinished(const std::shared_ptr<Kullo::Api::Session> &session)
{
    kulloAssert(session);

    session_ = session;
    eventDispatcher_.setSession(session);

    if (Applications::KulloApplication::FAKE_LONG_MIGRATION)
    {
        QTimer::singleShot(4000, this, SLOT(onInternalCreateSessionDone()));
    }
    else
    {
        onInternalCreateSessionDone();
    }
}

void Inbox::onCreateSessionError(
        const ApiMirror::SignalSlotValue<Kullo::Api::Address> &address,
        Kullo::Api::LocalError error)
{
    //TODO handle create session error
    K_UNUSED(address);
    K_UNUSED(error);
}

void Inbox::onInternalCreateSessionDone()
{
    kulloAssert(session_);

    Log.i() << "Logged in.";
    session_->syncer()->setListener(kulloForcedNn(syncerListener_));

    userSettingsModel_->setUserSettings(session_->userSettings());
    userSettingsModel_->migrate();

    conversationsSource_ = Kullo::make_unique<ConversationListSource>(eventDispatcher_, nullptr);
    connect(conversationsSource_.get(), &Qml::ConversationListSource::unreadMessagesCountChanged,
            this, &Inbox::unreadMessagesCountChanged);
    conversationsSource_->setSession(session_);

    conversationsAll_ = Kullo::make_unique<Qml::ConversationListSorted>(conversationsSource_.get(), nullptr);
    conversationsVisible_ = Kullo::make_unique<Qml::ConversationListSortedFiltered>(conversationsSource_.get(), nullptr);

    emit visibleConversationsChanged();

    emit hasSessionChanged(true);
}

void Inbox::onSyncProgressed(const ApiMirror::SignalSlotValue<Kullo::Api::SyncProgress> &progress)
{
    if (!latestSyncProgress_ || progress->phase != latestSyncProgress_->phase)
    {
        emit syncPhaseChanged(ApiMirror::Enums::SyncPhaseHolder::convert(progress->phase));
    }

    emit syncProgressed(
                ApiMirror::Enums::SyncPhaseHolder::convert(progress->phase),
                progress->incomingMessagesProcessed,
                progress->incomingMessagesTotal,
                progress->incomingMessagesNew,
                progress->incomingMessagesNewUnread,
                progress->incomingMessagesModified,
                progress->incomingMessagesDeleted,
                progress->incomingAttachmentsDownloadedBytes,
                progress->incomingAttachmentsTotalBytes,
                progress->outgoingMessagesUploadedBytes,
                progress->outgoingMessagesTotalBytes,
                progress->runTimeMs
                );

    latestSyncProgress_ = *progress;
}

void Inbox::onSyncFinished()
{
    kulloAssert(latestSyncProgress_);
    emit syncFinished(true,
                      latestSyncProgress_->incomingMessagesNew,
                      latestSyncProgress_->incomingMessagesNewUnread,
                      latestSyncProgress_->incomingMessagesModified,
                      latestSyncProgress_->incomingMessagesDeleted);
    latestSyncProgress_ = boost::none;
}

void Inbox::onSyncError(Kullo::Api::NetworkError error)
{
    using namespace Kullo::Api;

    switch (error)
    {
    case NetworkError::Forbidden:
        emit clientTooOld();
        break;
    case NetworkError::Protocol:
        emit clientTooOld();
        break;
    case NetworkError::Unauthorized:
        emit syncError(ApiMirror::Enums::NetworkErrorHolder::convert(error));
        break;
    case NetworkError::Server:
        emit syncError(ApiMirror::Enums::NetworkErrorHolder::convert(error));
        break;
    case NetworkError::Connection:
        emit syncError(ApiMirror::Enums::NetworkErrorHolder::convert(error));
        break;
    case NetworkError::Unknown:
        emit syncError(ApiMirror::Enums::NetworkErrorHolder::convert(error));
        if (Applications::KulloApplication::TEST_MODE)
        {
            std::terminate();
        }
        break;
    default: kulloAssert(false);
    }

    Log.w() << "Sync failed: " << error;
    emit syncFinished(false);

    latestSyncProgress_ = boost::none;
}

void Inbox::setLocalDatabaseKulloVersion(
        const Kullo::Api::Address &address,
        const DesktopUtil::KulloVersion &version)
{
    const QString key = QStringLiteral("kulloVersion-") + QString::fromStdString(address.toString());
    const QString value = QString::fromStdString(version.toString());

    QSettings settings;
    settings.beginGroup("database");
    settings.setValue(key, value);
    settings.endGroup();
}

}
}
