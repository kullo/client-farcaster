/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <apimirror/Client.h>
#include <apimirror/eventdispatcher.h>
#include <apimirror/SyncerListener.h>
#include <desktoputil/kulloversion.h>
#include <kulloclient/api/LocalError.h>
#include <kulloclient/api/SyncProgress.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class SyncErrors : public QObject
{
    Q_OBJECT

public:
    enum class SyncError
    {
        ServerError,
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
    explicit ClientModel(
            ApiMirror::Client &client,
            const QString &dbFilenameTemplate,
            Applications::KulloApplication &app,
            QObject *parent = 0);
    ~ClientModel() override;

    Q_PROPERTY(KulloDesktop::Qml::UserSettingsModel *userSettings READ userSettings NOTIFY userSettingsChanged)
    UserSettingsModel *userSettings();

    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    bool loggedIn() const;

    Q_PROPERTY(KulloDesktop::Qml::ConversationListModel *conversations READ conversations NOTIFY conversationsChanged)
    ConversationListModel *conversations();

    std::shared_ptr<ConversationListSource> conversationsListSource();

    Q_INVOKABLE void addConversation(QString participants);

    Q_INVOKABLE void removeConversation(Kullo::id_type conversationId);

    Q_INVOKABLE bool sync();

    Q_INVOKABLE void logIn();
    Q_INVOKABLE void logOut();
    Q_INVOKABLE void removeDatabase(const QString &addr);

    std::shared_ptr<ParticipantModel> participantModel(const std::shared_ptr<Kullo::Api::Address> &address) const;

    Q_INVOKABLE ApiMirror::Client *client() const;
    std::shared_ptr<Kullo::Api::Session> session() const;

signals:
    void userSettingsChanged();
    void conversationsChanged();
    void loggedInChanged(bool loggedIn);
    void draftAttachmentsTooBig(Kullo::id_type conversationId);
    void syncStarted();
    void syncProgressed(int countMessagesProcessed, int countMessagesTotal);
    void syncFinished(bool success,
                      int countMessagesNew = 0,
                      int countMessagesNewUnread = 0,
                      int countMessagesModified = 0,
                      int countMessagesDeleted = 0);
    void syncError(SyncErrors::SyncError error);
    void clientTooOld();
    void openConversation(Kullo::id_type conversationId);

private slots:
    void onCreateSessionFinished(const std::shared_ptr<Kullo::Api::Session> &session);
    void onCreateSessionError(const std::shared_ptr<Kullo::Api::Address> &address, Kullo::Api::LocalError error);
    void onInternalLoginDone();

    void onSyncProgressed(const std::shared_ptr<Kullo::Api::SyncProgress> &progress);
    void onSyncFinished();
    void onSyncError(Kullo::Api::NetworkError error);

private:
    void setLocalDatabaseKulloVersion(const std::shared_ptr<Kullo::Api::Address> &addr, const DesktopUtil::KulloVersion &version);
    DesktopUtil::KulloVersion getLocalDatabaseKulloVersion(const std::shared_ptr<Kullo::Api::Address> &addr);
    void prepareDatabasePath(const QString &dbFile);

    QString dbFilenameTemplate_;
    Applications::KulloApplication &app_;
    ApiMirror::EventDispatcher eventDispatcher_;
    std::shared_ptr<Kullo::Api::SyncProgress> latestSyncProgress_;
    std::shared_ptr<ApiMirror::SyncerListener> syncerListener_;
    ApiMirror::Client &client_;
    std::shared_ptr<Kullo::Api::Session> session_;
    std::shared_ptr<Qml::ConversationListSource> conversationsSource_;
    std::shared_ptr<Qml::ConversationListModel> conversationsProxy_;
    std::unique_ptr<UserSettingsModel> userSettingsModel_;

    std::shared_ptr<Kullo::Api::AsyncTask> createSessionTask_;

    Q_DISABLE_COPY(ClientModel)
};

}
}
