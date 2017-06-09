/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <boost/optional.hpp>

#include <apimirror/Client.h>
#include <apimirror/eventdispatcher.h>
#include <apimirror/SyncerListener.h>
#include <apimirror/enums/NetworkErrorHolder.h>
#include <apimirror/enums/SyncPhaseHolder.h>
#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/api/LocalError.h>
#include <kulloclient/api/SyncProgress.h>

#include "desktoputil/desktoputil-forwards.h"
#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

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

    Q_PROPERTY(bool hasSession READ hasSession NOTIFY hasSessionChanged)
    bool hasSession() const;

    Q_PROPERTY(KulloDesktop::Qml::ConversationListSorted *allConversations
               READ allConversations NOTIFY allConversationsChanged)
    ConversationListSorted *allConversations();

    Q_PROPERTY(KulloDesktop::Qml::ConversationListSortedFiltered *visibleConversations
               READ visibleConversations NOTIFY visibleConversationsChanged)
    ConversationListSortedFiltered *visibleConversations();

    Q_INVOKABLE void addConversation(QString participants);

    Q_INVOKABLE void removeConversation(Kullo::id_type conversationId);

    Q_INVOKABLE bool sync();

    // TODO: move this functionality to C++ to ensure critical operations are performed,
    // even if the user interface has problems.
    // This is already done in registration but not trivial in login
    Q_INVOKABLE void clearDatabaseAndStoreCredentials(const QString &addressString, const QString &masterKeyPem);

    Q_INVOKABLE void loadCredentials(const QString &addressString);
    Q_INVOKABLE void createSession();
    Q_INVOKABLE void closeSession();
    Q_INVOKABLE void deleteAccountData(const QString &addressString);

    Q_INVOKABLE QStringList allKnownUsersSorted() const;

    Q_INVOKABLE ApiMirror::Client *client() const;

    // non-QML
    std::shared_ptr<Sender> latestSenderForAddress(const std::shared_ptr<Kullo::Api::Address> &address) const;
    std::shared_ptr<Kullo::Api::Session> session() const;
    ConversationListSource *conversationsListSource();

signals:
    void migrationStarted();
    void userSettingsChanged();
    void allConversationsChanged();
    void visibleConversationsChanged();
    void hasSessionChanged(bool hasSession);
    void unreadMessagesCountChanged(int count);
    void draftPartTooBig(
            Kullo::id_type conversationId,
            ApiMirror::Enums::DraftPartHolder::DraftPart part,
            int64_t currentSize, int64_t maxSize);
    void syncStarted();
    void syncProgressed(
            ApiMirror::Enums::SyncPhaseHolder::SyncPhase phase,
            int incomingMessagesProcessed,
            int incomingMessagesTotal,
            int incomingMessagesNew,
            int incomingMessagesNewUnread,
            int incomingMessagesModified,
            int incomingMessagesDeleted,
            int incomingAttachmentsDownloadedBytes,
            int incomingAttachmentsTotalBytes,
            int outgoingMessagesUploadedBytes,
            int outgoingMessagesTotalBytes,
            int runTimeMs);
    void syncPhaseChanged(ApiMirror::Enums::SyncPhaseHolder::SyncPhase phase);
    void syncFinished(bool success,
                      int countMessagesNew = 0,
                      int countMessagesNewUnread = 0,
                      int countMessagesModified = 0,
                      int countMessagesDeleted = 0);
    void syncError(ApiMirror::Enums::NetworkErrorHolder::NetworkError error);
    void clientTooOld();
    void openConversation(Kullo::id_type conversationId);

private slots:
    void onCreateSessionMigrationStarted();
    void onCreateSessionFinished(const std::shared_ptr<Kullo::Api::Session> &session);
    void onCreateSessionError(const std::shared_ptr<Kullo::Api::Address> &address, Kullo::Api::LocalError error);
    void onInternalCreateSessionDone();

    void onSyncProgressed(const ApiMirror::SignalSlotValue<Kullo::Api::SyncProgress> &progress);
    void onSyncFinished();
    void onSyncError(Kullo::Api::NetworkError error);

private:
    void setLocalDatabaseKulloVersion(const std::shared_ptr<Kullo::Api::Address> &addr, const DesktopUtil::KulloVersion &version);

    InnerApplication &innerApplication_;
    Kullo::Util::StlTaskRunner *taskRunner_;
    ApiMirror::EventDispatcher eventDispatcher_;
    boost::optional<Kullo::Api::SyncProgress> latestSyncProgress_;
    std::shared_ptr<ApiMirror::SyncerListener> syncerListener_;
    ApiMirror::Client &client_;
    std::shared_ptr<Kullo::Api::Session> session_;
    std::unique_ptr<ConversationListSource> conversationsSource_;
    std::unique_ptr<ConversationListSorted> conversationsAll_;
    std::unique_ptr<ConversationListSortedFiltered> conversationsVisible_;
    std::unique_ptr<UserSettings> userSettingsModel_;

    std::shared_ptr<Kullo::Api::AsyncTask> createSessionTask_;

    Q_DISABLE_COPY(Inbox)
};

}
}
