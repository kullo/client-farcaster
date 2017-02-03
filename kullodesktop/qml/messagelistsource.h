/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QAbstractListModel>

#include <apimirror/eventdispatcher.h>
#include <kulloclient/types.h>
#include <kulloclient/api/Session.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class MessageListSource : public QAbstractListModel
{
    Q_OBJECT

public:
    enum MessageRoles {
        MessageIdRole = Qt::UserRole, // 256
        MessageTextRole,
        MessageTextAsHtmlRole,
        MessageDateSentRole,
        MessageDateReceivedRole,
        MessageReadRole,
        MessageDoneRole,
        ConversationIdRole,
        SenderAddressRole,
        SenderNameRole,
        SenderOrganizationRole,
        AttachmentsRole,
        AttachmentsReadyRole,
        FooterRole,
        DeliveryStatusRole,
        IncomingRole
    };

    MessageListSource(
            const std::shared_ptr<Kullo::Api::Session> &session,
            ApiMirror::EventDispatcher &eventDispatcher,
            Kullo::id_type convId,
            QObject *parent = nullptr);
    ~MessageListSource();
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void markAllMessagesAsRead();
    void markAllMessagesAsDone();
    void markAllMessagesAsReadAndDone();

    MessageModel *get(Kullo::id_type messageId) const;
    void deleteMessage(Kullo::id_type messageId);

private slots:
    void onMessageAdded(Kullo::id_type messageId);
    void onMessageRemoved(Kullo::id_type messageId);
    void onMessageAttachmentsDownloaded(Kullo::id_type messageId);
    void onMessageStateChanged(Kullo::id_type messageId);
    void onMessageDeliveryStatusChanged(Kullo::id_type messageId);

private:
    int getIndexForId(Kullo::id_type messageId) const;
    void loadMessageModels();
    void connectMessageModelToList(const std::unique_ptr<MessageModel> &mm);
    void insertMessage(const int position, std::unique_ptr<MessageModel> &&mm);

    std::shared_ptr<Kullo::Api::Session> session_;
    ApiMirror::EventDispatcher &eventDispatcher_;
    Kullo::id_type convId_ = -1;
    std::vector<std::unique_ptr<MessageModel>> messageModels_;
};

}
}
