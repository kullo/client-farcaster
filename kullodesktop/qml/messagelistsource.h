/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QAbstractListModel>

#include <desktoputil/dice/dice-forwards.h>
#include <desktoputil/dice/model/conversation.h>

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

    explicit MessageListSource(std::shared_ptr<Kullo::Model::Conversation> conv, QObject *parent);
    ~MessageListSource();
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void markAllMessagesAsRead();
    void markAllMessagesAsDone();
    void markAllMessagesAsReadAndDone();

    MessageModel *get(quint32 messageId) const;
    void deleteMessage(quint32 messageId);

private slots:
    void onMessageAdded(quint32 messageId);
    void onMessageDeleted(quint32 messageId);
    void onMessageAttachmentsDownloaded(quint32 messageId);
    void onMessageStateChanged(quint32 messageId);
    void onMessageDeliveryStatusChanged(quint32 messageId);

private:
    int getIndexForId(quint32 messageId) const;
    void loadMessageModels();
    void connectMessageModelToList(const std::unique_ptr<MessageModel> &mm);
    void insertMessage(const int position, std::unique_ptr<MessageModel> &&mm);

    std::shared_ptr<Kullo::Model::Conversation> conversation_;
    std::vector<std::unique_ptr<MessageModel>> messageModels_;
};

}
}
