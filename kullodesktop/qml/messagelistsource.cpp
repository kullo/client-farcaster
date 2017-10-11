/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "messagelistsource.h"

#include <QQmlEngine>

#include <kulloclient/api/Conversations.h>
#include <kulloclient/api/Messages.h>
#include <kulloclient/api/Session.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/messagemodel.h"

namespace KulloDesktop {
namespace Qml {

MessageListSource::MessageListSource(
        const std::shared_ptr<Kullo::Api::Session> &session,
        ApiMirror::EventDispatcher &eventDispatcher,
        Kullo::id_type convId,
        QObject *parent)
    : QAbstractListModel(parent)
    , session_(session)
    , eventDispatcher_(eventDispatcher)
    , convId_(convId)
{
    kulloAssert(session_);

    connect(&eventDispatcher, &ApiMirror::EventDispatcher::messageAdded,
            this, [this](Kullo::id_type conversationId, Kullo::id_type messageId)
    {
        if (conversationId == convId_) onMessageAdded(messageId);
    });
    connect(&eventDispatcher, &ApiMirror::EventDispatcher::messageRemoved,
            this, [this](Kullo::id_type conversationId, Kullo::id_type messageId)
    {
        if (conversationId == convId_) onMessageRemoved(messageId);
    });

    loadMessageModels();
}

MessageListSource::~MessageListSource()
{
}

QHash<int, QByteArray> MessageListSource::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MessageIdRole]           = QByteArrayLiteral("id_");
    roles[MessageTextRole]         = QByteArrayLiteral("text_");
    roles[MessageTextAsHtmlRole]   = QByteArrayLiteral("textAsHtml_");
    roles[MessageDateSentRole]     = QByteArrayLiteral("dateSent_");
    roles[MessageDateReceivedRole] = QByteArrayLiteral("dateReceived_");
    roles[MessageReadRole]         = QByteArrayLiteral("read_");
    roles[MessageDoneRole]         = QByteArrayLiteral("done_");
    roles[ConversationIdRole]      = QByteArrayLiteral("conversationId_");
    roles[SenderAddressRole]       = QByteArrayLiteral("senderAddress_");
    roles[SenderNameRole]          = QByteArrayLiteral("senderName_");
    roles[SenderOrganizationRole]  = QByteArrayLiteral("senderOrganization_");
    roles[AttachmentsRole]         = QByteArrayLiteral("attachments_");
    roles[AttachmentsReadyRole]    = QByteArrayLiteral("attachmentsReady_");
    roles[FooterRole]              = QByteArrayLiteral("footer_");
    roles[DeliveryStatusRole]      = QByteArrayLiteral("deliveryStatus_");

    // convenience roles
    roles[IncomingRole] = QByteArrayLiteral("incoming_");
    return roles;
}

namespace {
bool addressInParticipants(
        const std::string &address,
        const std::unordered_set<Kullo::Api::Address> &participants)
{
    for (const auto &participant : participants)
    {
        if (participant.toString() == address) return true;
    }
    return false;
}
}

QVariant MessageListSource::data(const QModelIndex &index, int role) const
{
    auto row = static_cast<unsigned int>(index.row());
    if (index.row() < 0 || row >= messageModels_.size())
    {
        return QVariant();
    }

    AttachmentListModel *alm = nullptr;

    switch (role)
    {
    case MessageIdRole:
        return QVariant::fromValue(messageModels_.at(row)->id());
    case MessageTextRole:
        return messageModels_.at(row)->text();
    case MessageTextAsHtmlRole:
        return messageModels_.at(row)->textAsHtml();
    case ConversationIdRole:
        return QVariant::fromValue(messageModels_.at(row)->conversationId());
    case MessageDateSentRole:
        return messageModels_.at(row)->dateSent();
    case MessageDateReceivedRole:
        return messageModels_.at(row)->dateReceived();
    case MessageReadRole:
        return messageModels_.at(row)->read();
    case MessageDoneRole:
        return messageModels_.at(row)->done();
    case SenderAddressRole:
        return messageModels_.at(row)->sender()->address();
    case SenderNameRole:
        return messageModels_.at(row)->sender()->name();
    case SenderOrganizationRole:
        return messageModels_.at(row)->sender()->organization();
    case AttachmentsRole:
        alm = messageModels_.at(row)->attachments();
        QQmlEngine::setObjectOwnership(alm, QQmlEngine::CppOwnership);
        return QVariant::fromValue(alm);
    case AttachmentsReadyRole:
        return messageModels_.at(row)->attachmentsReady();
    case FooterRole:
        return messageModels_.at(row)->footer();
    case DeliveryStatusRole:
        return messageModels_.at(row)->deliveryStatus();
    case IncomingRole:
        return addressInParticipants(
                    messageModels_.at(row)->sender()->address().toStdString(),
                    session_->conversations()->participants(convId_));
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
        m->setRead(true);
        m->setDone(true);
    }
}

MessageModel *MessageListSource::get(Kullo::id_type messageId) const
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

void MessageListSource::deleteMessage(Kullo::id_type messageId)
{
    int position = getIndexForId(messageId);
    if (position == -1) return;

    messageModels_.at(position)->deletePermanently();
}

void MessageListSource::onMessageAdded(Kullo::id_type messageId)
{
    std::unique_ptr<MessageModel> mm(new MessageModel(session_, eventDispatcher_, messageId));
    connectMessageModelToList(mm);
    insertMessage(0, std::move(mm));
}

void MessageListSource::onMessageRemoved(Kullo::id_type messageId)
{
    int position = getIndexForId(messageId);
    if (position == -1) return;

    beginRemoveRows(QModelIndex(), position, position);
    messageModels_.erase(messageModels_.begin() + position);
    endRemoveRows();
}

void MessageListSource::onMessageAttachmentsDownloaded(Kullo::id_type messageId)
{
    int position = getIndexForId(messageId);
    if (position == -1) return;

    QModelIndex rowIndex = createIndex(position, 0);
    emit dataChanged(rowIndex, rowIndex, QVector<int>{AttachmentsReadyRole});
}

void MessageListSource::onMessageStateChanged(Kullo::id_type messageId)
{
    int position = getIndexForId(messageId);
    if (position == -1) return;

    QModelIndex rowIndex = createIndex(position, 0);
    emit dataChanged(rowIndex, rowIndex, QVector<int>{
                         MessageReadRole,
                         MessageDoneRole
                     });
}

void MessageListSource::onMessageDeliveryStatusChanged(Kullo::id_type messageId)
{
    int position = getIndexForId(messageId);
    if (position == -1) return;

    QModelIndex rowIndex = createIndex(position, 0);
    emit dataChanged(rowIndex, rowIndex, QVector<int>{DeliveryStatusRole});
}

int MessageListSource::getIndexForId(Kullo::id_type messageId) const
{
    for (size_t i = 0; i < messageModels_.size(); ++i)
    {
        auto &model = messageModels_.at(i);
        kulloAssert(model);
        if (model->id() == messageId)
        {
            return i;
        }
    }
    return -1;
}

void MessageListSource::loadMessageModels()
{
    for (auto msgId : session_->messages()->allForConversation(convId_))
    {
        std::unique_ptr<MessageModel> mm(new MessageModel(session_, eventDispatcher_, msgId));
        kulloAssert(mm);
        connectMessageModelToList(mm);
        messageModels_.push_back(std::move(mm));
    }
}

void MessageListSource::connectMessageModelToList(const std::unique_ptr<MessageModel> &mm)
{
    //TODO invert this relationship: listen to this signal here and then notify MessageModel
    connect(mm.get(), &MessageModel::attachmentsDownloaded,
            this, &MessageListSource::onMessageAttachmentsDownloaded);
    connect(mm.get(), &MessageModel::stateChanged,
            this, &MessageListSource::onMessageStateChanged);
    connect(mm.get(), &MessageModel::deliveryStatusChanged,
            this, &MessageListSource::onMessageDeliveryStatusChanged);
}

void MessageListSource::insertMessage(const int position, std::unique_ptr<MessageModel> &&mm)
{
    kulloAssert(mm);

    auto parent = QModelIndex();
    beginInsertRows(parent, position, position);
    messageModels_.insert(messageModels_.begin()+position, std::move(mm));
    endInsertRows();
}

}
}
