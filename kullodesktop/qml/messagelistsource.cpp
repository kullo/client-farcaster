/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "messagelistsource.h"

#include <QQmlEngine>

#include <desktoputil/dice/model/message.h>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/messagemodel.h"

namespace KulloDesktop {
namespace Qml {

MessageListSource::MessageListSource(std::shared_ptr<Kullo::Model::Conversation> conv, QObject *parent)
    : QAbstractListModel(parent)
    , conversation_(conv)
{
    kulloAssert(conversation_);

    connect(conversation_.get(), &Kullo::Model::Conversation::messageAdded,
            this, &MessageListSource::onMessageAdded);
    connect(conversation_.get(), &Kullo::Model::Conversation::messageDeleted,
            this, &MessageListSource::onMessageDeleted);

    loadMessageModels();
}

MessageListSource::~MessageListSource()
{
}

QHash<int, QByteArray> MessageListSource::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MessageIdRole]          = QByteArrayLiteral("id_");
    roles[MessageTextRole]        = QByteArrayLiteral("text_");
    roles[MessageTextAsHtmlRole]  = QByteArrayLiteral("textAsHtml_");
    roles[MessageDateSentRole]    = QByteArrayLiteral("dateSent_");
    roles[MessageReadRole]        = QByteArrayLiteral("read_");
    roles[MessageDoneRole]        = QByteArrayLiteral("done_");
    roles[ConversationIdRole]     = QByteArrayLiteral("conversationId_");
    roles[SenderAddressRole]      = QByteArrayLiteral("senderAddress_");
    roles[SenderNameRole]         = QByteArrayLiteral("senderName_");
    roles[SenderOrganizationRole] = QByteArrayLiteral("senderOrganization_");
    roles[AttachmentsRole]        = QByteArrayLiteral("attachments_");
    roles[AttachmentsReadyRole]   = QByteArrayLiteral("attachmentsReady_");
    roles[FooterRole]             = QByteArrayLiteral("footer_");
    roles[DeliveryStatusRole]     = QByteArrayLiteral("deliveryStatus_");

    // convenience roles
    roles[IncomingRole]           = QByteArrayLiteral("incoming_");
    return roles;
}

QVariant MessageListSource::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || static_cast<unsigned int>(index.row()) >= messageModels_.size())
    {
        return QVariant();
    }

    AttachmentListModel *alm = nullptr;

    switch (role)
    {
    case MessageIdRole:
        return QVariant(messageModels_.at(index.row())->id());
    case MessageTextRole:
        return QVariant(messageModels_.at(index.row())->text());
    case MessageTextAsHtmlRole:
        return QVariant(messageModels_.at(index.row())->textAsHtml());
    case ConversationIdRole:
        return QVariant(messageModels_.at(index.row())->conversationId());
    case MessageDateSentRole:
        return QVariant(messageModels_.at(index.row())->dateSent());
    case MessageReadRole:
        return QVariant(messageModels_.at(index.row())->read());
    case MessageDoneRole:
        return QVariant(messageModels_.at(index.row())->done());
    case SenderAddressRole:
        return QVariant(messageModels_.at(index.row())->sender()->address());
    case SenderNameRole:
        return QVariant(messageModels_.at(index.row())->sender()->name());
    case SenderOrganizationRole:
        return QVariant(messageModels_.at(index.row())->sender()->organization());
    case AttachmentsRole:
        alm = messageModels_.at(index.row())->attachments();
        QQmlEngine::setObjectOwnership(alm, QQmlEngine::CppOwnership);
        return QVariant::fromValue(alm);
    case AttachmentsReadyRole:
        return QVariant(messageModels_.at(index.row())->attachmentsReady());
    case FooterRole:
        return QVariant(messageModels_.at(index.row())->footer());
    case DeliveryStatusRole:
        return QVariant(messageModels_.at(index.row())->deliveryStatus());
    case IncomingRole:
        if (conversation_->participants().count(
                    Kullo::Util::KulloAddress(messageModels_.at(index.row())->sender()->address().toStdString())
                    ))
        {
            return QVariant(true);
        }
        else {
            return QVariant(false);
        }
    default:
        Log.e() << "MessageListModel: Invalid role requested: " << role;
        return QVariant();
    }
}

bool MessageListSource::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role) {
    case MessageReadRole:
        messageModels_.at(index.row())->setRead(value.toBool());
        emit dataChanged(index, index, (QVector<int>(0) << MessageReadRole));
        return true;
    case MessageDoneRole:
        messageModels_.at(index.row())->setDone(value.toBool());
        emit dataChanged(index, index, (QVector<int>(0) << MessageDoneRole));
        return true;
    default:
        Log.e() << "MessageListModel.setData: Unknown role: " << role;
        return false;
    }
}

int MessageListSource::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return messageModels_.size();
}

void MessageListSource::markAllMessagesAsRead()
{
    for (const std::unique_ptr<MessageModel> &m : messageModels_)
    {
        m->setRead(true);
    }
}

void MessageListSource::markAllMessagesAsDone()
{
    for (const std::unique_ptr<MessageModel> &m : messageModels_)
    {
        m->setDone(true);
    }
}

void MessageListSource::markAllMessagesAsReadAndDone()
{
    for (const std::unique_ptr<MessageModel> &m : messageModels_)
    {
        m->markAsReadAndDone();
    }
}

MessageModel *MessageListSource::get(quint32 messageId) const
{
    int position = getIndexForId(messageId);
    if (position == -1)
    {
        Log.w() << "Couldn't find MessageModel for messageId: " << messageId;
        return nullptr;
    }

    MessageModel *out = messageModels_.at(position).get();
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

void MessageListSource::deleteMessage(quint32 messageId)
{
    int position = getIndexForId(messageId);
    if (position == -1) return;

    messageModels_.at(position)->deletePermanently();
}

void MessageListSource::onMessageAdded(quint32 messageId)
{
    auto msg = conversation_->messages()[messageId];
    std::unique_ptr<MessageModel> mm(new MessageModel(msg, nullptr));
    connectMessageModelToList(mm);
    insertMessage(0, std::move(mm));
}

void MessageListSource::onMessageDeleted(quint32 messageId)
{
    int position = getIndexForId(messageId);
    if (position == -1) return;

    beginRemoveRows(QModelIndex(), position, position);
    messageModels_.erase(messageModels_.begin() + position);
    endRemoveRows();
}

void MessageListSource::onMessageAttachmentsDownloaded(quint32 messageId)
{
    int position = getIndexForId(messageId);
    if (position == -1) return;

    QModelIndex rowIndex = createIndex(position, 0);
    emit dataChanged(rowIndex, rowIndex, QVector<int>{AttachmentsReadyRole});
}

void MessageListSource::onMessageStateChanged(quint32 messageId)
{
    int position = getIndexForId(messageId);
    if (position == -1) return;

    QModelIndex rowIndex = createIndex(position, 0);
    emit dataChanged(rowIndex, rowIndex, QVector<int>{
                         MessageReadRole,
                         MessageDoneRole
                     });
}

void MessageListSource::onMessageDeliveryStatusChanged(quint32 messageId)
{
    int position = getIndexForId(messageId);
    if (position == -1) return;

    QModelIndex rowIndex = createIndex(position, 0);
    emit dataChanged(rowIndex, rowIndex, QVector<int>{DeliveryStatusRole});
}

int MessageListSource::getIndexForId(quint32 messageId) const
{
    for (size_t i = 0; i < messageModels_.size(); ++i)
    {
        if (messageModels_.at(i)->id() == messageId)
        {
            return i;
        }
    }
    return -1;
}

void MessageListSource::loadMessageModels()
{
    for (auto &itr : conversation_->messages())
    {
        auto msg = itr.second;
        if (!msg->isDeleted())
        {
            std::unique_ptr<MessageModel> mm(new MessageModel(msg, nullptr));
            connectMessageModelToList(mm);
            messageModels_.push_back(std::move(mm));
        }
    }
}

void MessageListSource::connectMessageModelToList(const std::unique_ptr<MessageModel> &mm)
{
    connect(mm.get(), &MessageModel::attachmentsDownloaded,
            this, &MessageListSource::onMessageAttachmentsDownloaded);
    connect(mm.get(), &MessageModel::stateChanged,
            this, &MessageListSource::onMessageStateChanged);
    connect(mm.get(), &MessageModel::deliveryStatusChanged,
            this, &MessageListSource::onMessageDeliveryStatusChanged);
}

void MessageListSource::insertMessage(const int position, std::unique_ptr<MessageModel> &&mm)
{
    auto parent = QModelIndex();
    beginInsertRows(parent, position, position);
    messageModels_.insert(messageModels_.begin()+position, std::move(mm));
    endInsertRows();
}

}
}
