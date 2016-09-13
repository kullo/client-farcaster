/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "conversationlistsource.h"

#include <algorithm>
#include <QQmlEngine>

#include <kulloclient/api/Conversations.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include "kullodesktop/qml/conversationmodel.h"
#include "kullodesktop/qml/messagemodel.h"

namespace KulloDesktop {
namespace Qml {

ConversationListSource::ConversationListSource(ApiMirror::EventDispatcher &eventDispatcher, QObject *parent)
    : QAbstractListModel(parent)
    , eventDispatcher_(eventDispatcher)
{
    connect(&eventDispatcher_, &ApiMirror::EventDispatcher::conversationAdded,
            this, &ConversationListSource::onConversationAdded);
    connect(&eventDispatcher_, &ApiMirror::EventDispatcher::conversationChanged,
            this, &ConversationListSource::onConversationChanged);
    connect(&eventDispatcher_, &ApiMirror::EventDispatcher::conversationRemoved,
            this, &ConversationListSource::onConversationRemoved);
    connect(&eventDispatcher_, &ApiMirror::EventDispatcher::latestSenderChanged,
            this, &ConversationListSource::onParticipantChanged);
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
    auto row = static_cast<unsigned int>(index.row());
    if (index.row() < 0 || row >= conversationModels_.size())
    {
        return QVariant();
    }

    switch (role)
    {
    case IdRole:
        return QVariant::fromValue(conversationModels_.at(row)->id());
    case ParticipantsAddressesRole:
        return conversationModels_.at(row)->participantsAddresses();
    case ParticipantsListRole:
        return conversationModels_.at(row)->participantsList();
    case ParticipantsRole:
        return conversationModels_.at(row)->participants();
    case CountRole:
        return conversationModels_.at(row)->count();
    case CountUndoneRole:
        return conversationModels_.at(row)->countUndone();
    case CountUnreadRole:
        return conversationModels_.at(row)->countUnread();
    case LatestMessageTimestampRole:
        return conversationModels_.at(row)->latestMessageTimestamp();
    case DraftEmptyRole:
        return conversationModels_.at(row)->draftEmpty();
    case AvatarSrcRole:
        return QStringLiteral("image://conversationavatars/%1?%2")
                        .arg(conversationModels_.at(row)->id())
                        .arg(QTime(0, 0, 0, 0).msecsTo(QTime::currentTime()) // rnd = 0,...,86400000
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

int ConversationListSource::getIndexForId(Kullo::id_type conversationId) const
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

void ConversationListSource::setSession(const std::shared_ptr<Kullo::Api::Session> &session)
{
    session_ = session;

    /*
     * Refresh internal state
     * 1. conversationModels_
     * 2. openWhenCreated_
     */
    refreshConversations();
    openWhenCreated_.clear();
}

void ConversationListSource::onConversationAdded(Kullo::id_type conversationId)
{
    K_UNUSED(conversationId);

    // Insert at the end
    const auto positionAfterCurrentEnd = rowCount();
    beginInsertRows(QModelIndex(), positionAfterCurrentEnd, positionAfterCurrentEnd);

    auto cm = Kullo::make_unique<ConversationModel>(session_, eventDispatcher_, conversationId);
    connectConversationModelToList(cm);
    conversationModels_.push_back(std::move(cm));

    endInsertRows();
    emit conversationsChanged();
}

void ConversationListSource::onConversationChanged(Kullo::id_type conversationId)
{
    int position = getIndexForId(conversationId);

    if (position == -1)
    {
        Log.w() << "Conversation not found";
    }
    else
    {
        conversationModels_[position]->notifyChanged();

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

void ConversationListSource::onConversationRemoved(Kullo::id_type conversationId)
{
    K_UNUSED(conversationId);

    // TODO: Remove conversation efficiently
    refreshConversations();
}

void ConversationListSource::onParticipantChanged(Kullo::id_type conversationId)
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

void ConversationListSource::onSingleConversationCountUnreadChanged()
{
    const auto count = unreadMessagesCount();
    if (count != lastUnreadMessagesCount_)
    {
        lastUnreadMessagesCount_ = count;
        emit unreadMessagesCountChanged(count);
    }
}

void ConversationListSource::refreshConversations()
{
    beginResetModel();

    conversationModels_.clear();

    if (session_)
    {
        for (auto convId : session_->conversations()->all())
        {
            auto cm = Kullo::make_unique<ConversationModel>(session_, eventDispatcher_, convId);
            connectConversationModelToList(cm);
            conversationModels_.push_back(std::move(cm));
        }
    }

    endResetModel();
    emit conversationsChanged();
}

void ConversationListSource::connectConversationModelToList(const std::unique_ptr<ConversationModel> &cm)
{
    auto convId = cm->id();
    connect(cm->draft(), &DraftModel::emptyChanged,
            this, [this, convId](){ onConversationChanged(convId); });
    connect(cm.get(), &ConversationModel::countUnreadChanged,
            this, &ConversationListSource::onSingleConversationCountUnreadChanged);
}

Kullo::id_type ConversationListSource::openWhenCreated()
{
    if (openWhenCreated_.empty()) return -1;

    QStringList openWhenCreatedAddresses;
    for (const auto &addr : openWhenCreated_)
    {
        openWhenCreatedAddresses << QString::fromStdString(addr->toString());
    }
    openWhenCreatedAddresses.sort();

    Kullo::id_type conversationId = -1;
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

void ConversationListSource::setOpenWhenCreated(
        std::unordered_set<std::shared_ptr<Kullo::Api::Address>> participants)
{
    if (openWhenCreated_ == participants) return;
    openWhenCreated_ = participants;
}

qint32 ConversationListSource::unreadMessagesCount() const
{
    qint32 sum = 0;
    for (const auto &cm : conversationModels_)
    {
        sum += cm->countUnread();
    }
    return sum;
}

ConversationModel *ConversationListSource::get(Kullo::id_type conversationId) const
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
