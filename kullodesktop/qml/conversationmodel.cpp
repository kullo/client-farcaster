/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "conversationmodel.h"

#include <QQmlEngine>
#include <QStringList>

#include <desktoputil/kulloclient2qt.h>
#include <kulloclient/api/Address.h>
#include <kulloclient/api/Conversations.h>
#include <kulloclient/api/Messages.h>
#include <kulloclient/api/Senders.h>
#include <kulloclient/api/Session.h>
#include <kulloclient/api_impl/DateTime.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/sender.h"

namespace KulloDesktop {
namespace Qml {

ConversationModel::ConversationModel(QObject *parent)
    : QObject(parent)
{
    Log.f() << "Don't instantiate Conversation in QML.";
}

ConversationModel::ConversationModel(
        const std::shared_ptr<Kullo::Api::Session> &session,
        ApiMirror::EventDispatcher &eventDispatcher,
        Kullo::id_type convId,
        QObject *parent)
    : QObject(parent)
    , session_(session)
    , eventDispatcher_(&eventDispatcher)
    , convId_(convId)
    , draft_(session_, *eventDispatcher_, convId_)
{
    kulloAssert(session_);
    kulloAssert(eventDispatcher_);
}

Kullo::id_type ConversationModel::id() const
{
    return convId_;
}

QStringList ConversationModel::participantsAddresses() const
{
    QStringList addresses;
    for (const auto &addr : session_->conversations()->participants(convId_))
    {
        addresses << QString::fromStdString(addr->toString());
    }
    addresses.sort();
    return addresses;
}

QString ConversationModel::participantsList() const
{
    QStringList names;
    // Can't rewrite this to use a modern for loop because we need iter.key(),
    // not iter->key(). Here, QMap differs from std::map.
    auto ps = participants();
    for (auto iter = ps.begin(); iter != ps.end(); ++iter)
    {
        auto address = iter.key();
        auto name = iter.value().toString();
        names << (!name.isEmpty() ? name : address);
    }
    return names.join(", ");
}

QVariantMap ConversationModel::participants() const
{
    QVariantMap out;
    for (const auto &addr : session_->conversations()->participants(convId_))
    {
        auto senderMsgId = session_->messages()->latestForSender(addr);
        std::string senderName;
        if (senderMsgId >= 0)
        {
            senderName = session_->senders()->name(senderMsgId);
        }
        out.insert(QString::fromStdString(addr->toString()), QString::fromStdString(senderName));
    }
    return out;
}

qint32 ConversationModel::count() const
{
    return session_->conversations()->totalMessages(convId_);
}

qint32 ConversationModel::countUnread() const
{
    auto count = session_->conversations()->unreadMessages(convId_);
    kulloAssert(count >= 0 && count <= 1000000);
    return count;
}

qint32 ConversationModel::countUndone() const
{
    auto count = session_->conversations()->undoneMessages(convId_);
    kulloAssert(count >= 0 && count <= 1000000);
    return count;
}

qint32 ConversationModel::countIncoming() const
{
    auto count = session_->conversations()->incomingMessages(convId_);
    kulloAssert(count >= 0 && count <= 1000000);
    return count;
}

qint32 ConversationModel::countOutgoing() const
{
    auto count = session_->conversations()->outgoingMessages(convId_);
    kulloAssert(count >= 0 && count <= 1000000);
    return count;
}

QDateTime ConversationModel::latestMessageTimestamp() const
{
    auto timestamp = session_->conversations()->latestMessageTimestamp(convId_);
    return DesktopUtil::KulloClient2Qt::toQDateTime(timestamp);
}

MessageListModel *ConversationModel::messages()
{
    // lazy loading
    if (!messages_)
    {
        messages_.reset(new MessageListModel(session_, *eventDispatcher_, convId_, nullptr));
        kulloAssert(messages_);
    }

    auto out = messages_.get();
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

DraftModel *ConversationModel::draft()
{
    auto out = &draft_;
    QQmlEngine::setObjectOwnership(out, QQmlEngine::CppOwnership);
    return out;
}

bool ConversationModel::draftEmpty()
{
    return draft_.empty();
}

void ConversationModel::markAllMessagesAsRead()
{
    messages()->markAllMessagesAsRead();
}

void ConversationModel::markAllMessagesAsDone()
{
    messages()->markAllMessagesAsDone();
}

void ConversationModel::notifyChanged()
{
    emit countChanged();
    emit countUnreadChanged();
    emit countUndoneChanged();
    emit draftEmptyChanged();
}

}
}
