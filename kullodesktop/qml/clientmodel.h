/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <desktoputil/dice/model/client.h>
#include <desktoputil/kulloversion.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class SyncErrors : public QObject
{
    Q_OBJECT

public:
    enum class SyncError
    {
        InternalServerError,
        NetworkError,
        Unauthorized,
        UnknownError
    };
    Q_ENUM(SyncError)

    static void init();
};

class ClientModel : public QObject
{
    Q_OBJECT

public:
    explicit ClientModel(const QString &dbFilenameTemplate, Applications::KulloApplication &app, QObject *parent = 0);
    ~ClientModel();

    Q_PROPERTY(KulloDesktop::Qml::UserSettingsModel *userSettings READ userSettings NOTIFY userSettingsChanged)
    UserSettingsModel *userSettings();

    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    bool loggedIn() const;

    Q_PROPERTY(KulloDesktop::Qml::ConversationListModel *conversations READ conversations NOTIFY conversationsChanged)
    ConversationListModel *conversations();

    std::shared_ptr<ConversationListSource> conversationsListSource();

    Q_INVOKABLE void addConversation(QString participants);

    Q_INVOKABLE bool removeConversation(quint32 conversationId);

    Q_INVOKABLE bool sync();

    Q_INVOKABLE void prepareLogin();
    Q_INVOKABLE void logIn();
    Q_INVOKABLE void logOut();
    Q_INVOKABLE void removeDatabase(const QString &addr);

    std::map<Kullo::Util::KulloAddress, std::shared_ptr<ParticipantModel>> participantsModels() const;

    Kullo::Model::Client *client() const;

signals:
    void userSettingsChanged();
    void conversationsChanged();
    void loggedInChanged(bool loggedIn);
    void conversationAdded(Kullo::id_type conversationId);
    void conversationRemoved(Kullo::id_type conversationId);
    void draftAttachmentsTooBig(Kullo::id_type conversationId, std::size_t size, std::size_t sizeAllowed);
    void syncStarted();
    void syncProgressend(int countMessagesProcessed, int countMessagesTotal);
    void syncFinished(bool success,
                      int countMessagesNew = 0,
                      int countMessagesNewUnread = 0,
                      int countMessagesModified = 0,
                      int countMessagesDeleted = 0);
    void syncError(SyncErrors::SyncError error);
    void clientTooOld();
    void loginPrepared();
    void openConversation(Kullo::id_type conversationId);

private slots:
    void onSyncProgressed(const Kullo::Sync::SyncProgress &progress);
    void onSyncFinished(const Kullo::Sync::SyncProgress &progress);
    void onSyncError(std::exception_ptr exptr);

private:
    void setLocalDatabaseKulloVersion(const Kullo::Util::KulloAddress &addr, const DesktopUtil::KulloVersion &version);
    DesktopUtil::KulloVersion getLocalDatabaseKulloVersion(const Kullo::Util::KulloAddress &addr);
    void prepareDatabasePath(const QString &dbFile);

    QString dbFilenameTemplate_;
    Applications::KulloApplication &app_;
    std::unique_ptr<Kullo::Model::Client> client_;
    std::shared_ptr<Qml::ConversationListSource> conversationsSource_;
    std::shared_ptr<Qml::ConversationListModel> conversationsProxy_;
    std::unique_ptr<UserSettingsModel> userSettingsModel_;

    Q_DISABLE_COPY(ClientModel)
};

}
}
