/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "conversation.h"

#include <ostream>
#include <kulloclient/dao/conversationdao.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/librarylogger.h>

#include "desktoputil/stlqt.h"
#include "desktoputil/dice/model/client.h"
#include "desktoputil/dice/model/draft.h"
#include "desktoputil/dice/model/exceptions.h"
#include "desktoputil/dice/model/message.h"
#include "desktoputil/dice/model/participant.h"

using namespace Kullo::Util;

namespace Kullo {
namespace Model {

const DateTime Conversation::EMPTY_CONVERSATION_TIMESTAMP = DateTime(2222, 2, 22, 22, 22, 22);

id_type Conversation::id() const
{
    return dao_->id();
}

std::map<KulloAddress, std::shared_ptr<Participant> > Conversation::participants()
{
    auto out = std::map<KulloAddress, std::shared_ptr<Participant>>();
    auto participantsGlobal = client_->participants();
    for (const auto &addressStr : dao_->participantsList())
    {
        const auto address = KulloAddress(addressStr);
        if (participantsGlobal.count(address))
        {
            out[address] = participantsGlobal[address];
        }
        else
        {
            out[address] = std::make_shared<Participant>(client_, address, nullptr, nullptr);
        }
    }
    return out;
}

size_t Conversation::countMessages(const Dao::MessageState state) const
{
    kulloAssert(messageCountsLoaded_);
    const auto count = messageCounts_.at(state);
    kulloAssert(count <= 1000000);
    return count;
}

void Conversation::deletePermanently()
{
    dao_->deletePermanently();
}

Util::DateTime Conversation::latestMessageTimestamp() const
{
    kulloAssert(latestMessageTimestampLoaded_);
    return latestMessageTimestamp_;
}

std::map<id_type, std::shared_ptr<Message> > Conversation::messages()
{
    // lazy loading
    if (!messagesLoaded_) loadMessages();
    return messages_;
}

std::shared_ptr<Draft> Conversation::draft()
{
    //lazy loading
    if (!draft_)
    {
        draft_ = std::make_shared<Draft>(client_, dao_->id());
    }
    return draft_;
}

bool Conversation::operator<(const Conversation &other) const
{
    return latestMessageTimestamp() < other.latestMessageTimestamp();
}

bool Conversation::operator>(const Conversation &other) const
{
    return latestMessageTimestamp() > other.latestMessageTimestamp();
}

Conversation::Conversation(Client *client, Dao::ConversationDao *dao, QObject *parent)
    : QObject(parent),
      client_(client),
      dao_(dao)
{
    kulloAssert(parent == nullptr);
    kulloAssert(client_);
    kulloAssert(dao_);
    updateMeta();
}

Conversation::Conversation(Client *client, id_type id, QObject *parent)
    : QObject(parent),
      client_(client)
{
    kulloAssert(parent == nullptr);
    kulloAssert(client_);

    dao_ = Dao::ConversationDao::load(id, client_->session());
    if (!dao_)
    {
        throw NotFound("Conversation::Conversation(): "
                       "conversation could not be found by id");
    }
    updateMeta();
}

Conversation::Conversation(Client *client, std::set<KulloAddress> participants, QObject *parent)
    : QObject(parent),
      client_(client)
{
    kulloAssert(parent == nullptr);
    kulloAssert(client_);

    dao_ = Dao::ConversationDao::load(participants, client_->session());
    if (!dao_)
    {
        throw NotFound("Conversation::Conversation(): "
                       "conversation could not be found by participants");
    }
    updateMeta();
}

Conversation::~Conversation()
{
}

void Conversation::loadMessages()
{
    Log.d() << "Conversation::loadMessages(), id = " << id();
    auto result = Dao::MessageDao::messagesForConversation(dao_->id(), client_->session());
    while (auto msgDao = result->next())
    {
        auto msg = std::make_shared<Message>(client_, msgDao.release());
        addMessage(msg);
    }
    messagesLoaded_ = true;
}

void Conversation::onDraftModified()
{
    Log.d() << "Calling Conversation::onDraftModified() ...";
    if (!draft_) return;

    draft_->reload();
}

void Conversation::onMessageAdded(id_type messageId)
{
    if (messagesLoaded_)
    {
        if (messages_.count(messageId) != 0)
        {
            Log.e() << "Message with same id already exists in conversation. messageId = " << messageId;
            return;
        }

        auto msg = std::make_shared<Message>(client_, messageId);
        addMessage(msg);
        updateMeta(msg);
    }
    else
    {
        updateMeta();
    }
    emit messageAdded(messageId);
}

void Conversation::onMessageDeleted(const id_type messageId)
{
    if (!messagesLoaded_) return;

    messages_.erase(messageId);
    updateMeta();
    emit messageDeleted(messageId);
}

void Conversation::onMessageStateChanged(const id_type messageId)
{
    if (messagesLoaded_ && messages_.count(messageId))
        updateMeta(messages_[messageId]);
    else
        updateMeta();

    emit messageStateChanged(messageId);
}

void Conversation::onMessageModified(id_type messageId)
{
    if (messagesLoaded_ && messages_.count(messageId))
    {
        auto msg = messages_[messageId];
        kulloAssert(msg);
        msg->reloadState();

        updateMeta(msg);
    }
    else
    {
        updateMeta();
    }

    emit messageStateChanged(messageId);
}

void Conversation::onMessageAttachmentsDownloaded(id_type messageId)
{
    if (!messagesLoaded_) return;

    if (messages_.count(messageId))
    {
        auto msg = messages_[messageId];
        kulloAssert(msg);

        msg->onAttachmentsDownloaded();
    }
}

void Conversation::onConversationModified()
{
    id_type id = dao_->id();
    dao_ = Dao::ConversationDao::load(id, client_->session());
    kulloAssert(dao_);

    emit modified();
}

std::ostream &operator<<(std::ostream &out, const Conversation &conv)
{
    out << "Conversation("
        << "id=" << conv.id() << ", "
        << "latest=" << conv.latestMessageTimestamp() << ", "
        << ")";
    return out;
}

void Conversation::addMessage(std::shared_ptr<Message> msg)
{
    messages_[msg->id()] = msg;
    connect(msg.get(), &Message::deleted, this, &Conversation::onMessageDeleted);
    connect(msg.get(), &Message::stateChanged, this, &Conversation::onMessageStateChanged);
}

void Conversation::reloadLatestMessageTimestamp()
{
    const std::string latestMessageTimestampString = dao_->loadLastMessageTime();
    latestMessageTimestamp_ = (!latestMessageTimestampString.empty())
            ? Util::DateTime(latestMessageTimestampString)
            : EMPTY_CONVERSATION_TIMESTAMP;
    latestMessageTimestampLoaded_ = true;
}

void Conversation::updateLatestMessageTimestamp(std::shared_ptr<Message> msg)
{
    latestMessageTimestamp_ = std::max(msg->dateSent(), latestMessageTimestamp_);
}

void Conversation::reloadMessageCounts()
{
    size_t any  = 0;
    size_t read = 0;
    size_t unread = 0;
    size_t done = 0;
    size_t undone = 0;
    if (messagesLoaded_)
    {
        any = messages_.size();
        for (const auto &idMsgPair : messages_)
        {
            auto msg = idMsgPair.second;
            kulloAssert(msg);
            if (msg->state(Dao::MessageState::Read))   ++read;
            if (msg->state(Dao::MessageState::Unread)) ++unread;
            if (msg->state(Dao::MessageState::Done))   ++done;
            if (msg->state(Dao::MessageState::Undone)) ++undone;
        }
    }
    else
    {
        any    = dao_->loadMessageCount(Dao::MessageState::Any);
        read   = dao_->loadMessageCount(Dao::MessageState::Read);
        unread = dao_->loadMessageCount(Dao::MessageState::Unread);
        done   = dao_->loadMessageCount(Dao::MessageState::Done);
        undone = dao_->loadMessageCount(Dao::MessageState::Undone);
    }
    messageCounts_[Dao::MessageState::Any]    = any;
    messageCounts_[Dao::MessageState::Read]   = read;
    messageCounts_[Dao::MessageState::Unread] = unread;
    messageCounts_[Dao::MessageState::Done]   = done;
    messageCounts_[Dao::MessageState::Undone] = undone;
    messageCountsLoaded_ = true;
}

void Conversation::updateMeta(std::shared_ptr<Message> msg)
{
    reloadMessageCounts();
    if (msg) updateLatestMessageTimestamp(msg);
    else     reloadLatestMessageTimestamp();
}

}
}
