/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <apimirror/Client.h>
#include <apimirror/eventdispatcher.h>
#include <apimirror/SyncerListener.h>
#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/api/LocalError.h>
#include <kulloclient/api/SyncProgress.h>

#include "desktoputil/desktoputil-forwards.h"
#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

// This is used as Class.EnumValue in QML (e.g. SyncErrors.Unauthorized),
// so the enum name is skipped.
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

    static void registerEnumsInClassForSignalSlot();
};

class Inbox : public QObject
{
    Q_OBJECT

public:
    explicit Inbox(InnerApplication &innerApplication,
                   ApiMirror::Client &client,
                   Kullo::Util::StlTaskRunner *taskRunner,
                   QObject *parent = 0);
    ~Inbox() override;

    Q_PROPERTY(KulloDesktop::Qml::UserSettings *userSettings READ userSettings NOTIFY userSettingsChanged)
    UserSettings *userSettings();

    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    bool loggedIn() const;

    Q_PROPERTY(KulloDesktop::Qml::ConversationListModel *conversations READ conversations NOTIFY conversationsChanged)
    ConversationListModel *conversations();

    Q_INVOKABLE void addConversation(QString participants);

    Q_INVOKABLE void removeConversation(Kullo::id_type conversationId);

    Q_INVOKABLE bool sync();

    // TODO: move this functionality to C++ to ensure critical operations are performed,
    // even if the user interface has problems.
    // This is already done in registration but not trivial in login
    Q_INVOKABLE void clearDatabaseAndStoreCredentials(const QString &addressString, const QString &masterKeyPem);

    Q_INVOKABLE void loadCredentials(const QString &addressString);
    Q_INVOKABLE void logIn();
    Q_INVOKABLE void logOut();

    Q_INVOKABLE ApiMirror::Client *client() const;

    // non-QML
    std::shared_ptr<Sender> latestSenderForAddress(const std::shared_ptr<Kullo::Api::Address> &address) const;
    std::shared_ptr<Kullo::Api::Session> session() const;
    ConversationListSource *conversationsListSource();

signals:
    void userSettingsChanged();
    void conversationsChanged();
    void loggedInChanged(bool loggedIn);
    void unreadMessagesCountChanged(int count);
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

    InnerApplication &innerApplication_;
    Kullo::Util::StlTaskRunner *taskRunner_;
    ApiMirror::EventDispatcher eventDispatcher_;
    std::shared_ptr<Kullo::Api::SyncProgress> latestSyncProgress_;
    std::shared_ptr<ApiMirror::SyncerListener> syncerListener_;
    ApiMirror::Client &client_;
    std::shared_ptr<Kullo::Api::Session> session_;
    std::unique_ptr<ConversationListSource> conversationsSource_;
    std::unique_ptr<ConversationListModel> conversationsProxy_;
    std::unique_ptr<UserSettings> userSettingsModel_;

    std::shared_ptr<Kullo::Api::AsyncTask> createSessionTask_;

    Q_DISABLE_COPY(Inbox)
};

}
}
