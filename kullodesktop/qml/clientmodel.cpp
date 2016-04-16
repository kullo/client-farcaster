/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "clientmodel.h"

#include <exception>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QQmlEngine>
#include <QSettings>
#include <QTimer>

#include <apimirror/ClientCreateSessionListener.h>
#include <apimirror/SessionListener.h>
#include <desktoputil/asynctask.h>
#include <desktoputil/kulloaddresshelper.h>
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

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/qml/conversationlistmodel.h"
#include "kullodesktop/qml/conversationlistsource.h"
#include "kullodesktop/qml/participantmodel.h"
#include "kullodesktop/qml/usersettingsmodel.h"

namespace KulloDesktop {
namespace Qml {

void SyncErrors::init()
{
    qRegisterMetaType<SyncErrors::SyncError>("SyncErrors::SyncError");
}

ClientModel::ClientModel(
        ApiMirror::Client &client,
        const QString &dbFilenameTemplate,
        Applications::KulloApplication &app,
        QObject *parent)
    : QObject(parent)
    , dbFilenameTemplate_(dbFilenameTemplate)
    , app_(app)
    , syncerListener_(std::make_shared<ApiMirror::SyncerListener>())
    , client_(client)
    , userSettingsModel_(new UserSettingsModel())
{
    SyncErrors::init();

    QString activeUser = app_.deviceSettings().activeUser();
    if (!activeUser.isEmpty())  // load UserSettings if we have an active user
    {
        userSettingsModel_->load(activeUser);
    }

    connect(syncerListener_.get(), &ApiMirror::SyncerListener::_progressed,
            this, &ClientModel::onSyncProgressed);
    connect(syncerListener_.get(), &ApiMirror::SyncerListener::_finished,
            this, &ClientModel::onSyncFinished);
    connect(syncerListener_.get(), &ApiMirror::SyncerListener::_error,
            this, &ClientModel::onSyncError);
    connect(syncerListener_.get(), &ApiMirror::SyncerListener::_draftAttachmentsTooBig,
            this, &ClientModel::draftAttachmentsTooBig);
}

ClientModel::~ClientModel()
{
}

bool ClientModel::loggedIn() const
{
    return session_ != nullptr;
}

ConversationListModel *ClientModel::conversations()
{
    auto ptr = conversationsProxy_.get();
    QQmlEngine::setObjectOwnership(ptr, QQmlEngine::CppOwnership);
    return ptr;
}

void ClientModel::logIn()
{
    if (loggedIn()) return;

    // Set active user on login
    auto addr = userSettingsModel_->address();
    app_.deviceSettings().setActiveUser(addr);
    app_.deviceSettings().setLastActiveUser(addr);

    QString dbFilePath = dbFilenameTemplate_.arg(addr);
    prepareDatabasePath(dbFilePath);

    // Begin Database Version Check
    auto currentUserAddress = Kullo::Api::Address::create(addr.toStdString());
    DesktopUtil::KulloVersion currentKulloVersion = DesktopUtil::Versions::kullodesktopVersion();
    DesktopUtil::KulloVersion localDatabaseKulloVersion = getLocalDatabaseKulloVersion(currentUserAddress);
    Log.i() << "Database Kullo version installed: " << localDatabaseKulloVersion << " "
            << "running: " << currentKulloVersion;
    if (localDatabaseKulloVersion != currentKulloVersion)
    {
        setLocalDatabaseKulloVersion(currentUserAddress, currentKulloVersion);
    }
    // End Database Version Check

    Log.i() << "Logging in using database file " << dbFilePath << " ...";

    auto sessionListener = std::make_shared<ApiMirror::SessionListener>();
    connect(sessionListener.get(), &ApiMirror::SessionListener::_internalEvent,
            &eventDispatcher_, &ApiMirror::EventDispatcher::onInternalEvent);

    auto listener = std::make_shared<ApiMirror::ClientCreateSessionListener>();
    connect(listener.get(), &ApiMirror::ClientCreateSessionListener::_finished,
            this, &ClientModel::onCreateSessionFinished);
    connect(listener.get(), &ApiMirror::ClientCreateSessionListener::_error,
            this, &ClientModel::onCreateSessionError);

    createSessionTask_ = client_.raw()->createSessionAsync(
                userSettingsModel_->rawUserSettings(),
                dbFilePath.toStdString(),
                sessionListener,
                listener
                );
}

void ClientModel::logOut()
{
    if (!loggedIn()) return;

    Log.i() << "Logging out ...";
    session_.reset();
    app_.deviceSettings().setActiveUser("");
    conversationsSource_->setSession(nullptr);

    emit loggedInChanged(false);
}

void ClientModel::removeDatabase(const QString &addr)
{
    if (!DesktopUtil::KulloAddressHelper::isValidKulloAddress(addr))
    {
        Log.e() << "Invalid Kullo address: " << addr;
        return;
    }

    const QString dbFile = dbFilenameTemplate_.arg(addr);
    const QString dbFileShm = dbFile + "-shm";
    const QString dbFileWal = dbFile + "-wal";

    Log.d() << "Removing database file " << dbFile << " ...";

    if (QFile::remove(dbFile))    Log.i() << "Removed database file: " << dbFile;
    if (QFile::remove(dbFileShm)) Log.i() << "Removed database file: " << dbFileShm;
    if (QFile::remove(dbFileWal)) Log.i() << "Removed database file: " << dbFileWal;
}

std::shared_ptr<ParticipantModel> ClientModel::participantModel(
        const std::shared_ptr<Kullo::Api::Address> &address) const
{
    if (!session_) return nullptr;

    auto msgId = session_->messages()->latestForSender(address);
    if (msgId < 0) return nullptr;
    return std::make_shared<ParticipantModel>(session_, msgId);
}

UserSettingsModel *ClientModel::userSettings()
{
    auto out = userSettingsModel_.get();
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

std::shared_ptr<ConversationListSource> ClientModel::conversationsListSource()
{
    if (!conversationsSource_)
    {
        conversationsSource_ = std::make_shared<Qml::ConversationListSource>(eventDispatcher_, nullptr);
    }
    return conversationsSource_;
}

void ClientModel::addConversation(QString participants)
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

void ClientModel::removeConversation(Kullo::id_type conversationId)
{
    if (session_)
    {
        session_->conversations()->remove(conversationId);
    }
}

bool ClientModel::sync()
{
    if (!loggedIn()) return false;

    auto syncAlreadyRunning = session_->syncer()->isSyncing();
    session_->syncer()->requestSync(Kullo::Api::SyncMode::Everything);
    if (!syncAlreadyRunning) emit syncStarted();
    return true;
}

ApiMirror::Client *ClientModel::client() const
{
    QQmlEngine::setObjectOwnership(&client_, QQmlEngine::CppOwnership);
    return &client_;
}

std::shared_ptr<Kullo::Api::Session> ClientModel::session() const
{
    return session_;
}

void ClientModel::onCreateSessionFinished(const std::shared_ptr<Kullo::Api::Session> &session)
{
    kulloAssert(session);

    session_ = session;
    eventDispatcher_.setSession(session);

    if (app_.FAKE_LONG_MIGRATION)
    {
        QTimer::singleShot(4000, this, SLOT(onInternalLoginDone()));
    }
    else
    {
        onInternalLoginDone();
    }
}

void ClientModel::onCreateSessionError(const std::shared_ptr<Kullo::Api::Address> &address, Kullo::Api::LocalError error)
{
    //TODO handle create session error
}

void ClientModel::onInternalLoginDone()
{
    kulloAssert(session_);

    Log.i() << "Logged in.";
    session_->syncer()->setListener(syncerListener_);

    conversationsListSource()->setSession(session_);
    conversationsProxy_ = std::make_shared<Qml::ConversationListModel>(conversationsSource_, nullptr);
    emit conversationsChanged();

    emit loggedInChanged(true);
}

void ClientModel::onSyncProgressed(const std::shared_ptr<Kullo::Api::SyncProgress> &progress)
{
    latestSyncProgress_ = progress;
    emit syncProgressed(latestSyncProgress_->countProcessed,
                         latestSyncProgress_->countTotal);
}

void ClientModel::onSyncFinished()
{
    kulloAssert(latestSyncProgress_);
    emit syncFinished(true,
                      latestSyncProgress_->countNew,
                      latestSyncProgress_->countNewUnread,
                      latestSyncProgress_->countModified,
                      latestSyncProgress_->countDeleted);
}

void ClientModel::onSyncError(Kullo::Api::NetworkError error)
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
        emit syncError(SyncErrors::SyncError::Unauthorized);
        break;
    case NetworkError::Server:
        emit syncError(SyncErrors::SyncError::ServerError);
        break;
    case NetworkError::Connection:
        emit syncError(SyncErrors::SyncError::NetworkError);
        break;
    case NetworkError::Unknown:
        emit syncError(SyncErrors::SyncError::UnknownError);
        if (Applications::KulloApplication::TEST_MODE)
        {
            std::terminate();
        }
        break;
    default: kulloAssert(false);
    }

    Log.w() << "Sync failed: " << error;
    emit syncFinished(false);
}

void ClientModel::setLocalDatabaseKulloVersion(const std::shared_ptr<Kullo::Api::Address> &addr, const DesktopUtil::KulloVersion &version)
{
    const QString key = QStringLiteral("kulloVersion-") + QString::fromStdString(addr->toString());
    const QString value = QString::fromStdString(version.toString());

    QSettings settings;
    settings.beginGroup("database");
    settings.setValue(key, value);
    settings.endGroup();
}

DesktopUtil::KulloVersion ClientModel::getLocalDatabaseKulloVersion(const std::shared_ptr<Kullo::Api::Address> &addr)
{
    QString localDatabaseKulloVersion;
    QSettings settings;
    settings.beginGroup("database");
    QVariant data = settings.value(QStringLiteral("kulloVersion-") + QString::fromStdString(addr->toString()));
    if (data.isValid()) localDatabaseKulloVersion = data.toString();
    settings.endGroup();

    if (localDatabaseKulloVersion.isEmpty())
    {
        return DesktopUtil::KulloVersion("0.0.0");
    }
    else
    {
        return DesktopUtil::KulloVersion(localDatabaseKulloVersion.toStdString());
    }
}

/*
 * Create parent direcory for `dbFile` if necessary
 * and check write access
 */
void ClientModel::prepareDatabasePath(const QString &dbFile)
{
    QFileInfo dbFileInfo(dbFile);
    // TODO: Make check compatible with symbolic links
    if (!dbFileInfo.absoluteDir().exists())
    {
        Log.i() << "Creating database directory: " << dbFileInfo.absolutePath();
        if (!QDir().mkpath(dbFileInfo.absolutePath()))
        {
            Log.f() << "Could not create database directory: " << dbFileInfo.absolutePath();
        }
    }
    if (!QFileInfo(dbFileInfo.absolutePath()).isWritable())
    {
        Log.f() << "Database directory not writeable: " << dbFileInfo.absolutePath();
    }
    if (dbFileInfo.exists() && !dbFileInfo.isWritable())
    {
        Log.f() << "Database file not writeable: " << dbFileInfo.absoluteFilePath();
    }
}

}
}
