/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "conversationlistsource.h"

#include <algorithm>
#include <QQmlEngine>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include "kullodesktop/qml/clientmodel.h"
#include "kullodesktop/qml/conversationmodel.h"
#include "kullodesktop/qml/messagemodel.h"

namespace KulloDesktop {
namespace Qml {

ConversationListSource::ConversationListSource(ClientModel *clientModel, QObject *parent)
    : QAbstractListModel(parent)
    , clientModel_(clientModel)
{
    kulloAssert(clientModel_);

    connect(clientModel_, &ClientModel::conversationAdded,
            this, &ConversationListSource::onConversationAdded);
    connect(clientModel_, &ClientModel::conversationRemoved,
            this, &ConversationListSource::onConversationRemoved);
}

ConversationListSource::~ConversationListSource()
{
}

QHash<int, QByteArray> ConversationListSource::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]                     = QByteArrayLiteral("id_");
    roles[ParticipantsAddressesRole]  = QByteArrayLiteral("participantsAddresses_");
    roles[ParticipantsListRole]       = QByteArrayLiteral("participantsList_");
    roles[ParticipantsRole]           = QByteArrayLiteral("participants_");
    roles[CountRole]                  = QByteArrayLiteral("count_");
    roles[CountUndoneRole]            = QByteArrayLiteral("countUndone_");
    roles[CountUnreadRole]            = QByteArrayLiteral("countUnread_");
    roles[LatestMessageTimestampRole] = QByteArrayLiteral("latestMessageTimestamp_");
    roles[DraftEmptyRole]             = QByteArrayLiteral("draftEmpty_");
    roles[AvatarSrcRole]              = QByteArrayLiteral("avatarSrc_");
    return roles;
}

QVariant ConversationListSource::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || static_cast<unsigned int>(index.row()) >= conversationModels_.size())
    {
        return QVariant();
    }

    switch (role)
    {
    case IdRole:
        return QVariant(conversationModels_.at(index.row())->id());
    case ParticipantsAddressesRole:
        return QVariant(conversationModels_.at(index.row())->participantsAddresses());
    case ParticipantsListRole:
        return QVariant(conversationModels_.at(index.row())->participantsList());
    case ParticipantsRole:
        return QVariant(conversationModels_.at(index.row())->participants());
    case CountRole:
        return QVariant(conversationModels_.at(index.row())->count());
    case CountUndoneRole:
        return QVariant(conversationModels_.at(index.row())->countUndone());
    case CountUnreadRole:
        return QVariant(conversationModels_.at(index.row())->countUnread());
    case LatestMessageTimestampRole:
        return QVariant(conversationModels_.at(index.row())->latestMessageTimestamp());
    case DraftEmptyRole:
        return QVariant(conversationModels_.at(index.row())->draftEmpty());
    case AvatarSrcRole:
        return QVariant(QStringLiteral("image://conversationavatars/%1?%2")
                        .arg(conversationModels_.at(index.row())->id())
                        .arg(QTime(0, 0, 0, 0).msecsTo(QTime::currentTime())) // rnd = 0,...,86400000
        );
    default:
        Log.e() << "ConversationList::data(): Unknown role: " << role;
        return QVariant();
    }
}

int ConversationListSource::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return conversationModels_.size();
}

int ConversationListSource::getIndexForId(quint32 conversationId) const
{
    for (size_t i = 0; i < conversationModels_.size(); ++i)
    {
        if (conversationModels_.at(i)->id() == conversationId)
        {
            return i;
        }
    }
    return -1;
}

/*
 * Refresh internal state
 * 1. conversationModels_
 * 2. openWhenCreated_
 */
void ConversationListSource::refresh()
{
    refreshConversations();
    openWhenCreated_.clear();
}

void ConversationListSource::onConversationAdded(quint32 conversationId)
{
    K_UNUSED(conversationId);

    // Insert at the end
    const auto positionAfterCurrentEnd = rowCount();
    beginInsertRows(QModelIndex(), positionAfterCurrentEnd, positionAfterCurrentEnd);

    const std::shared_ptr<Kullo::Model::Conversation> conv = clientModel_->client()->conversations().at(conversationId);
    auto cm = Kullo::make_unique<ConversationModel>(conv, nullptr);
    connectConversationModelToList(cm);
    conversationModels_.push_back(std::move(cm));

    endInsertRows();
    emit conversationsChanged();
}

void ConversationListSource::onConversationRemoved(quint32 conversationId)
{
    K_UNUSED(conversationId);

    // TODO: Remove conversation efficiently
    refreshConversations();
}

void ConversationListSource::onConversationModified(quint32 conversationId)
{
    int position = getIndexForId(conversationId);

    if (position == -1)
    {
        Log.w() << "Conversation not found";
    }
    else
    {
        QModelIndex rowIndex = createIndex(position, 0);
        emit dataChanged(rowIndex, rowIndex,
                         QVector<int>{
                             ParticipantsListRole,
                             CountRole,
                             CountUndoneRole,
                             CountUnreadRole,
                             DraftEmptyRole,
                             LatestMessageTimestampRole
                         });
        emit conversationsChanged();
    }
}

void ConversationListSource::onParticipantChanged(quint32 conversationId)
{
    int position = getIndexForId(conversationId);

    if (position == -1)
    {
        Log.w() << "Conversation not found";
    }
    else
    {
        QModelIndex rowIndex = createIndex(position, 0);
        emit dataChanged(rowIndex, rowIndex, QVector<int>{AvatarSrcRole});
    }
}

void ConversationListSource::refreshConversations()
{
    beginResetModel();

    conversationModels_.clear();

    kulloAssert(clientModel_->client());
    if (clientModel_->loggedIn())
    {
        for (const auto &idConvPair : clientModel_->client()->conversations())
        {
            std::shared_ptr<Kullo::Model::Conversation> conv = idConvPair.second;
            auto cm = Kullo::make_unique<ConversationModel>(conv, nullptr);
            connectConversationModelToList(cm);
            conversationModels_.push_back(std::move(cm));
        }
    }

    endResetModel();
    emit conversationsChanged();
}

void ConversationListSource::connectConversationModelToList(const std::unique_ptr<ConversationModel> &cm)
{
    connect(cm.get(), &ConversationModel::conversationModified,
            this, &ConversationListSource::onConversationModified);
    connect(cm.get(), &ConversationModel::participantChanged,
            this, &ConversationListSource::onParticipantChanged);
    connect(cm.get(), &ConversationModel::countUnreadChanged,
            this, &ConversationListSource::unreadMessagesCountChanged);
}

quint32 ConversationListSource::openWhenCreated()
{
    if (openWhenCreated_.empty()) return 0;

    QStringList openWhenCreatedAddresses;
    for (const Kullo::Util::KulloAddress &addr : openWhenCreated_)
    {
        openWhenCreatedAddresses << QString::fromStdString(addr.toString());
    }
    openWhenCreatedAddresses.sort();

    quint32 conversationId = 0;
    for (const std::unique_ptr<ConversationModel> &cm : conversationModels_)
    {
        if (openWhenCreatedAddresses == cm->participantsAddresses())
        {
            conversationId = cm->id();
            break;
        }
    }

    // Reset
    openWhenCreated_.clear();

    return conversationId;
}

void ConversationListSource::setOpenWhenCreated(std::set<Kullo::Util::KulloAddress> participants)
{
    if (openWhenCreated_ == participants) return;
    openWhenCreated_ = participants;
}

quint32 ConversationListSource::unreadMessagesCount()
{
    quint32 sum = 0;
    for (const auto &cm : conversationModels_)
    {
        sum += cm->countUnread();
    }
    return sum;
}

ConversationModel *ConversationListSource::get(quint32 conversationId) const
{
    for (const std::unique_ptr<ConversationModel> &cm : conversationModels_)
    {
        if (cm->id() == conversationId)
        {
            auto out = cm.get();
            QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
            return out;
        }
    }
    return nullptr;
}

}
}
