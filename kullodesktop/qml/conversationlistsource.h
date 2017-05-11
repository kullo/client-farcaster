/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <unordered_set>
#include <QAbstractListModel>

#include <apimirror/eventdispatcher.h>
#include <kulloclient/kulloclient-forwards.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class ConversationListSource : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ConversationRoles {
        IdRole = Qt::UserRole, // 256
        ParticipantsAddressesRole,
        ParticipantsListRole,
        CountRole,
        CountUndoneRole,
        CountUnreadRole,
        LatestMessageTimestampRole,
        DraftEmptyRole,
    };

    explicit ConversationListSource(ApiMirror::EventDispatcher &eventDispatcher, QObject *parent = nullptr);
    ~ConversationListSource();
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Kullo::id_type openWhenCreated();
    void setOpenWhenCreated(std::unordered_set<std::shared_ptr<Kullo::Api::Address>> participants);

    qint32 unreadMessagesCount() const;

    ConversationModel *get(Kullo::id_type conversationId) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int getIndexForId(Kullo::id_type conversationId) const;

    void setSession(const std::shared_ptr<Kullo::Api::Session> &session);

signals:
    void conversationsChanged();
    void unreadMessagesCountChanged(int count);

private slots:
    void onConversationAdded(Kullo::id_type conversationId);
    void onConversationRemoved(Kullo::id_type conversationId);
    void onConversationChanged(Kullo::id_type conversationId);
    void onConversationLatestMessageTimestampChanged(Kullo::id_type conversationId);
    void onSingleConversationCountUnreadChanged();

private:
    void refreshConversations();
    void connectConversationModelToList(const std::unique_ptr<ConversationModel> &cm);

    ApiMirror::EventDispatcher &eventDispatcher_;
    std::shared_ptr<Kullo::Api::Session> session_;
    std::vector<std::unique_ptr<ConversationModel>> conversationModels_;
    std::unordered_set<std::shared_ptr<Kullo::Api::Address>> openWhenCreated_;
    int lastUnreadMessagesCount_ = -1;
};

}
}
