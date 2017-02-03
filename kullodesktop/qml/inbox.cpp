/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "inbox.h"

#include <exception>
#include <QDir>
#include <QFile>
#include <QQmlEngine>
#include <QSettings>
#include <QTimer>

#include <apimirror/ClientCreateSessionListener.h>
#include <apimirror/SessionListener.h>
#include <desktoputil/asynctask.h>
#include <desktoputil/databasefiles.h>
#include <desktoputil/kulloversion.h>
#include <desktoputil/qtypestreamers.h>
#include <desktoputil/versions.h>
#include <kulloclient/api/Address.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/Client.h>
#include <kulloclient/api/Conversations.h>
#include <kulloclient/api/MasterKey.h>
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
#include "kullodesktop/qml/conversationlistmodel.h"
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
        userSettingsModel_ = UserSettings::loadCredentialsForAddress(activeUser);
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
    connect(this, &Inbox::loggedInChanged,
            &innerApplication_, &InnerApplication::loggedInChanged);
    connect(this, &Inbox::unreadMessagesCountChanged,
            &innerApplication_, &InnerApplication::unreadMessagesCountChanged);
    connect(this, &Inbox::syncFinished,
            &innerApplication_, &InnerApplication::syncFinished);
}

Inbox::~Inbox()
{
}

bool Inbox::loggedIn() const
{
    return session_ != nullptr;
}

ConversationListModel *Inbox::conversations()
{
    auto ptr = conversationsProxy_.get();
    QQmlEngine::setObjectOwnership(ptr, QQmlEngine::CppOwnership);
    return ptr;
}

void Inbox::logIn()
{
    if (loggedIn()) return;

    // Set active user on login
    auto addr = userSettingsModel_->address();
    kulloAssert(!addr.isEmpty());
    innerApplication_.deviceSettings()->setActiveUser(addr);
    innerApplication_.deviceSettings()->setLastActiveUser(addr);

    auto currentUserAddress = Kullo::Api::Address::create(addr.toStdString());

    auto dbFilePath = innerApplication_.databaseFiles().databaseFilepath(currentUserAddress);
    innerApplication_.databaseFiles().prepareDatabaseFolder(currentUserAddress);
    Log.i() << "Logging in using database file " << dbFilePath << " ...";

    setLocalDatabaseKulloVersion(currentUserAddress, DesktopUtil::KulloVersion("0.0.0"));

    auto sessionListener = std::make_shared<ApiMirror::SessionListener>();
    connect(sessionListener.get(), &ApiMirror::SessionListener::_internalEvent,
            &eventDispatcher_, &ApiMirror::EventDispatcher::onInternalEvent);

    auto listener = std::make_shared<ApiMirror::ClientCreateSessionListener>();
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
                );

    if (Applications::KulloApplication::FAKE_LONG_MIGRATION)
    {
        QTimer::singleShot(1000, this, SLOT(onCreateSessionMigrationStarted()));
    }
}

void Inbox::logOut()
{
    if (!loggedIn()) return;

    Log.i() << "Logging out ...";

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
    conversationsProxy_ = nullptr;
    conversationsSource_ = nullptr;

    createSessionTask_ = nullptr;

    session_ = nullptr;
    innerApplication_.deviceSettings()->setActiveUser("");
    userSettingsModel_ = nullptr;

    taskRunner_->reset();

    emit loggedInChanged(false);
}

UserSettings *Inbox::userSettings()
{
    auto out = userSettingsModel_.get();
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

void Inbox::addConversation(QString participants)
{
    std::unordered_set<std::shared_ptr<Kullo::Api::Address>> participantsSet;
    foreach (QString address, participants.trimmed().split(','))
    {
        participantsSet.insert(Kullo::Api::Address::create(address.toStdString()));
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
    if (!loggedIn()) return false;

    auto syncAlreadyRunning = session_->syncer()->isSyncing();
    session_->syncer()->requestSync(Kullo::Api::SyncMode::Everything);
    if (!syncAlreadyRunning) emit syncStarted();
    return true;
}

void Inbox::clearDatabaseAndStoreCredentials(const QString &addressString, const QString &masterKeyPem)
{
    const auto address = Kullo::Api::Address::create(addressString.toStdString());
    const auto masterKey = Kullo::Api::MasterKey::createFromPem(masterKeyPem.toStdString());

    // Clear old user stuff
    innerApplication_.databaseFiles().removeDatabase(address);

    // Store new credentials
    UserSettings::storeCredentials(address, masterKey);
}

void Inbox::loadCredentials(const QString &addressString)
{
    userSettingsModel_ = UserSettings::loadCredentialsForAddress(addressString);
    emit userSettingsChanged();
}

ApiMirror::Client *Inbox::client() const
{
    QQmlEngine::setObjectOwnership(&client_, QQmlEngine::CppOwnership);
    return &client_;
}

std::shared_ptr<Sender> Inbox::latestSenderForAddress(
        const std::shared_ptr<Kullo::Api::Address> &address) const
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
        QTimer::singleShot(4000, this, SLOT(onInternalLoginDone()));
    }
    else
    {
        onInternalLoginDone();
    }
}

void Inbox::onCreateSessionError(const std::shared_ptr<Kullo::Api::Address> &address, Kullo::Api::LocalError error)
{
    //TODO handle create session error
    K_UNUSED(address);
    K_UNUSED(error);
}

void Inbox::onInternalLoginDone()
{
    kulloAssert(session_);

    Log.i() << "Logged in.";
    session_->syncer()->setListener(syncerListener_);

    userSettingsModel_->setUserSettings(session_->userSettings());
    userSettingsModel_->migrate();

    conversationsSource_ = Kullo::make_unique<ConversationListSource>(eventDispatcher_, nullptr);
    connect(conversationsSource_.get(), &Qml::ConversationListSource::unreadMessagesCountChanged,
            this, &Inbox::unreadMessagesCountChanged);
    conversationsSource_->setSession(session_);

    conversationsProxy_ = Kullo::make_unique<Qml::ConversationListModel>(conversationsSource_.get(), nullptr);

    emit conversationsChanged();

    emit loggedInChanged(true);
}

void Inbox::onSyncProgressed(const std::shared_ptr<Kullo::Api::SyncProgress> &progress)
{
    kulloAssert(progress);

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

void Inbox::setLocalDatabaseKulloVersion(const std::shared_ptr<Kullo::Api::Address> &addr, const DesktopUtil::KulloVersion &version)
{
    const QString key = QStringLiteral("kulloVersion-") + QString::fromStdString(addr->toString());
    const QString value = QString::fromStdString(version.toString());

    QSettings settings;
    settings.beginGroup("database");
    settings.setValue(key, value);
    settings.endGroup();
}

}
}
