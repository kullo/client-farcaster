/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "clientmodel.h"

#include <exception>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QQmlEngine>
#include <QSettings>

#include <desktoputil/asynctask.h>
#include <desktoputil/kulloaddresshelper.h>
#include <desktoputil/qtypestreamers.h>
#include <desktoputil/versions.h>
#include <kulloclient/protocol/exceptions.h>
#include <kulloclient/sync/definitions.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/exceptions.h>
#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/stacktrace.h>
#include <kulloclient/util/version.h>

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

ClientModel::ClientModel(const QString &dbFilenameTemplate, Applications::KulloApplication &app, QObject *parent)
    : QObject(parent)
    , dbFilenameTemplate_(dbFilenameTemplate)
    , app_(app)
    , client_(Kullo::make_unique<Kullo::Model::Client>())
    , userSettingsModel_(new UserSettingsModel(&client_->userSettings(), nullptr))
{
    SyncErrors::init();

    QString activeUser = app_.deviceSettings().activeUser();
    if (!activeUser.isEmpty())  // empty user profile. Do nothing
    {
        userSettingsModel_->load(activeUser);
    }

    connect(client_.get(), &Kullo::Model::Client::syncProgressed,
            this, &ClientModel::onSyncProgressed);
    connect(client_.get(), &Kullo::Model::Client::syncFinished,
            this, &ClientModel::onSyncFinished);
    connect(client_.get(), &Kullo::Model::Client::syncError,
            this, &ClientModel::onSyncError);
    connect(client_.get(), &Kullo::Model::Client::conversationAdded,
            this, &ClientModel::conversationAdded);
    connect(client_.get(), &Kullo::Model::Client::conversationRemoved,
            this, &ClientModel::conversationRemoved);
    connect(client_.get(), &Kullo::Model::Client::draftAttachmentsTooBig,
            this, &ClientModel::draftAttachmentsTooBig);
}

ClientModel::~ClientModel()
{
}

bool ClientModel::loggedIn() const
{
    return client_->loggedIn();
}

ConversationListModel *ClientModel::conversations()
{
    auto out = conversationsProxy_.get();
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

void ClientModel::prepareLogin()
{
    if (loggedIn()) return;

    // Set active user on login
    app_.deviceSettings().setActiveUser(userSettingsModel_->address());
    app_.deviceSettings().setLastActiveUser(userSettingsModel_->address());

    QString dbFile = dbFilenameTemplate_.arg(userSettingsModel_->address());
    prepareDatabasePath(dbFile);

    // Begin Database Version Check
    Kullo::Util::KulloAddress currentUserAddress = Kullo::Util::KulloAddress(userSettingsModel_->address().toStdString());
    DesktopUtil::KulloVersion currentKulloVersion = DesktopUtil::Versions::kullodesktopVersion();
    DesktopUtil::KulloVersion localDatabaseKulloVersion = getLocalDatabaseKulloVersion(currentUserAddress);
    Log.i() << "Database Kullo version installed: " << localDatabaseKulloVersion << " "
            << "running: " << currentKulloVersion;
    if (localDatabaseKulloVersion != currentKulloVersion)
    {
        setLocalDatabaseKulloVersion(currentUserAddress, currentKulloVersion);
    }
    // End Database Version Check

    auto login = [&](const QString dbFile, const bool fakeLongMigration)
    {
        Log.i() << "Preparing login using database file " << dbFile << " ...";
        client_->prepareLogIn(dbFile.toStdString());
        if (fakeLongMigration)
        {
            std::this_thread::sleep_for(std::chrono::seconds(4));
        }
        Log.d() << "Login prepared.";
        emit loginPrepared();
    };
    DesktopUtil::AsyncTask::runDetached(std::bind(login, dbFile, app_.FAKE_LONG_MIGRATION));
}

void ClientModel::logIn()
{
    client_->logIn();
    Log.i() << "Logged in.";

    conversationsSource_ = std::make_shared<Qml::ConversationListSource>(this, nullptr);
    conversationsSource_->refresh();
    conversationsProxy_ = std::make_shared<Qml::ConversationListModel>(conversationsSource_, nullptr);
    emit conversationsChanged();

    emit loggedInChanged(true);
}

void ClientModel::logOut()
{
    if (!loggedIn()) return;

    Log.i() << "Logging out ...";
    client_->logOut();
    app_.deviceSettings().setActiveUser("");
    conversationsSource_->refresh();
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

UserSettingsModel *ClientModel::userSettings()
{
    auto out = userSettingsModel_.get();
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

std::shared_ptr<ConversationListSource> ClientModel::conversationsListSource()
{
    return conversationsSource_;
}

void ClientModel::addConversation(QString participants)
{
    std::set<Kullo::Util::KulloAddress> participants_set;
    foreach (QString address, participants.trimmed().split(','))
    {
        participants_set.insert(Kullo::Util::KulloAddress(address.toStdString()));
    }

    if (!participants_set.empty())
    {
        auto convId = client_->findConversation(participants_set);
        if (convId == Kullo::Model::Client::NOT_FOUND)
        {
            conversationsSource_->setOpenWhenCreated(participants_set);
            client_->startConversation(participants_set);
        }
        else
        {
            emit openConversation(convId);
        }
    }
}

bool ClientModel::removeConversation(quint32 conversationId)
{
    return client_->removeConversation(conversationId);
}

bool ClientModel::sync()
{
    if (!loggedIn()) return false;

    bool started = client_->sync();
    if (started) emit syncStarted();
    return started;
}

std::map<Kullo::Util::KulloAddress, std::shared_ptr<ParticipantModel> > ClientModel::participantsModels() const
{
    std::map<Kullo::Util::KulloAddress, std::shared_ptr<ParticipantModel>> participants;
    for (const auto &addrParticipantPair : client_->participants())
    {
        auto address = addrParticipantPair.first;
        auto part = addrParticipantPair.second;
        participants.emplace(address, std::make_shared<ParticipantModel>(part, nullptr));
    }
    return participants;
}

Kullo::Model::Client *ClientModel::client() const
{
    return client_.get();
}

void ClientModel::onSyncProgressed(const Kullo::Sync::SyncProgress &progress)
{
    emit syncProgressend(progress.messages.countProcessed,
                         progress.messages.countTotal);
}

void ClientModel::onSyncFinished(const Kullo::Sync::SyncProgress &progress)
{
    emit syncFinished(true,
                      progress.messages.countNew,
                      progress.messages.countNewUnread,
                      progress.messages.countModified,
                      progress.messages.countDeleted);
}

void ClientModel::onSyncError(std::exception_ptr exptr)
{
    try
    {
        std::rethrow_exception(exptr);
    }
    catch (Kullo::Protocol::Forbidden &ex)
    {
        Log.w() << "Sync failed: " << Kullo::Util::formatException(ex);
        emit clientTooOld();
    }
    catch (Kullo::Protocol::InternalServerError &ex)
    {
        Log.w() << "Sync failed: " << Kullo::Util::formatException(ex);
        emit syncError(SyncErrors::SyncError::InternalServerError);
    }
    catch (Kullo::Protocol::NetworkError &ex)
    {
        Log.w() << "Sync failed: " << Kullo::Util::formatException(ex);
        emit syncError(SyncErrors::SyncError::NetworkError);
    }
    catch (Kullo::Protocol::Unauthorized &ex)
    {
        Log.w() << "Sync failed: " << Kullo::Util::formatException(ex);
        emit syncError(SyncErrors::SyncError::Unauthorized);
    }
    catch (std::exception &ex)
    {
        Log.e() << "Sync failed: " << Kullo::Util::formatException(ex);
        emit syncError(SyncErrors::SyncError::UnknownError);

        if (Applications::KulloApplication::TEST_MODE)
        {
            std::terminate();
        }
    }
    emit syncFinished(false);
}

void ClientModel::setLocalDatabaseKulloVersion(const Kullo::Util::KulloAddress &addr, const DesktopUtil::KulloVersion &version)
{
    const QString key = QStringLiteral("kulloVersion-") + QString::fromStdString(addr.toString());
    const QString value = QString::fromStdString(version.toString());

    QSettings settings;
    settings.beginGroup("database");
    settings.setValue(key, value);
    settings.endGroup();
}

DesktopUtil::KulloVersion ClientModel::getLocalDatabaseKulloVersion(const Kullo::Util::KulloAddress &addr)
{
    QString localDatabaseKulloVersion;
    QSettings settings;
    settings.beginGroup("database");
    QVariant data = settings.value(QStringLiteral("kulloVersion-") + QString::fromStdString(addr.toString()));
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
