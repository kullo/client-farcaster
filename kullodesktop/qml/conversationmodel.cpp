/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "conversationmodel.h"

#include <QQmlEngine>
#include <QStringList>

#include <desktoputil/kulloclient2qt.h>
#include <desktoputil/dice/dice-forwards.h>
#include <desktoputil/dice/model/participant.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/participantmodel.h"

namespace KulloDesktop {
namespace Qml {

ConversationModel::ConversationModel(QObject *parent)
    : QObject(parent)
{
    Log.e() << "Don't instantiate Conversation in QML.";
}

ConversationModel::ConversationModel(std::shared_ptr<Kullo::Model::Conversation> conv, QObject *parent)
    : QObject(parent)
    , conv_(conv)
    , draft_(conv->draft(), nullptr)
{
    kulloAssert(conv_);
    kulloAssert(parent == nullptr);

    // Emit signals so that the ConversationListModel can do resorting
    connect(conv_.get(), &Kullo::Model::Conversation::messageAdded,   this, &ConversationModel::onMessageAdded);
    connect(conv_.get(), &Kullo::Model::Conversation::messageDeleted, this, &ConversationModel::onMessageDeleted);

    // Conversation infos changed (e.g. countRead, countDone, count, draft status)
    connect(conv_.get(), &Kullo::Model::Conversation::messageAdded,        this, &ConversationModel::onConversationModified);
    connect(conv_.get(), &Kullo::Model::Conversation::messageDeleted,      this, &ConversationModel::onConversationModified);
    connect(conv_.get(), &Kullo::Model::Conversation::messageStateChanged, this, &ConversationModel::onConversationModified);
    connect(conv_.get(), &Kullo::Model::Conversation::modified,            this, &ConversationModel::onConversationModified);
    connect(&draft_,     &DraftModel::emptyChanged,                  this, &ConversationModel::onConversationModified);

    for (const auto &addrParticipantPair : conv_->participants())
    {
        std::shared_ptr<Kullo::Model::Participant> part = addrParticipantPair.second;
        connect(part.get(), &Kullo::Model::Participant::changed,
                this, &ConversationModel::onParticipantChanged);
    }
}

quint32 ConversationModel::id() const
{
    return conv_->id();
}

QStringList ConversationModel::participantsAddresses() const
{
    QStringList addresses;
    for (const auto &addrParticipantPair : conv_->participants())
    {
        auto part = addrParticipantPair.second;
        addresses << QString::fromStdString(part->address().toString());
    }
    addresses.sort();
    return addresses;
}

QString ConversationModel::participantsList() const
{
    QStringList names;
    for (const auto &addrParticipantPair : conv_->participants())
    {
        auto part = addrParticipantPair.second;
        std::string name = !part->name().empty() ? part->name() : part->address().toString();
        names << QString::fromStdString(name);
    }
    return names.join(", ");
}

QVariantMap ConversationModel::participants() const
{
    QVariantMap out;
    for (const auto &addrParticipantPair : conv_->participants())
    {
        auto address = QString::fromStdString(addrParticipantPair.first.toString());
        auto name    = QString::fromStdString(addrParticipantPair.second->name());
        out.insert(address, name);
    }
    return out;
}

quint32 ConversationModel::count() const
{
    const size_t any = conv_->countMessages(Kullo::Dao::MessageState::Any);
    return any;
}

quint32 ConversationModel::countUnread() const
{
    const size_t unread  = conv_->countMessages(Kullo::Dao::MessageState::Unread);
    kulloAssert(unread <= 1000000);
    return unread;
}

quint32 ConversationModel::countUndone() const
{
    const size_t undone  = conv_->countMessages(Kullo::Dao::MessageState::Undone);
    kulloAssert(undone <= 1000000);
    return undone;
}

QDateTime ConversationModel::latestMessageTimestamp() const
{
    return DesktopUtil::KulloClient2Qt::toQDateTime(conv_->latestMessageTimestamp());
}

MessageListModel *ConversationModel::messages()
{
    // lazy loading
    if (!messages_)
    {
        messages_.reset(new MessageListModel(conv_, nullptr));
        kulloAssert(messages_);
    }

    auto *out = messages_.get();
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

std::vector<std::unique_ptr<ParticipantModel>> ConversationModel::participantsModels() const
{
    std::vector<std::unique_ptr<ParticipantModel>> participants;
    for (const auto &addrParticipantPair : conv_->participants())
    {
        auto part = addrParticipantPair.second;
        participants.emplace_back(new ParticipantModel(part, nullptr));
    }
    return participants;
}

void ConversationModel::onParticipantChanged()
{
    emit participantChanged(id());
}

void ConversationModel::onConversationModified()
{
    quint32 conversationId = id();
    emit conversationModified(conversationId);
    emit countChanged();
    emit countUnreadChanged();
    emit countUndoneChanged();
    emit draftEmptyChanged();
}

void ConversationModel::onMessageAdded(quint32 messageId)
{
    emit messageAdded(id(), messageId);
}

void ConversationModel::onMessageDeleted(quint32 messageId)
{
    emit messageDeleted(id(), messageId);
}

}
}
