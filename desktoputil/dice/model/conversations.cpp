/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "conversations.h"

#include <memory>
#include <kulloclient/dao/conversationdao.h>
#include <kulloclient/dao/participantdao.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "desktoputil/dice/model/client.h"
#include "desktoputil/dice/model/conversation.h"
#include "desktoputil/dice/model/draft.h"
#include "desktoputil/dice/model/participant.h"

using namespace Kullo::Util;

namespace Kullo {
namespace Model {

Conversations::Conversations(Client *client, Db::SharedSessionPtr session, QObject *parent) :
    QObject(parent),
    client_(client),
    session_(session)
{
    kulloAssert(parent == nullptr);
    kulloAssert(client_);
    kulloAssert(session_);
}

Conversations::~Conversations()
{
}

std::map<id_type, std::shared_ptr<Conversation>> Conversations::conversations()
{
    // lazy loading of conversations
    if (!conversationsLoaded_)
    {
        Log.d() << "Loading conversations list ...";
        auto allConversationsResult = Dao::ConversationDao::all(session_);
        while (auto dao = allConversationsResult->next())
        {
            auto conv = std::make_shared<Conversation>(client_, dao.release());
            conversations_[conv->id()] = conv;
        }
        conversationsLoaded_ = true;
    }

    return conversations_;
}

id_type Conversations::findConversation(const std::set<KulloAddress> &participants)
{
    auto ret = Client::NOT_FOUND;
    for (const auto &idConversationPair : conversations())
    {
        const auto convId = idConversationPair.first;
        const auto conversation = idConversationPair.second;

        auto conversationAddresses = std::set<KulloAddress>();
        for (const auto addressParticipantPair : conversation->participants())
        {
            conversationAddresses.insert(addressParticipantPair.first);
        }

        if (conversationAddresses == participants)
        {
            ret = convId;
            break;
        }
    }

    return ret;
}

id_type Conversations::startConversation(const std::set<KulloAddress> &participants)
{
    kulloAssert(!participants.empty());
    kulloAssert(participants.count(*client_->userSettings().address) == 0);

    // only create new conversation if it doesn't exist yet
    auto convDao = Dao::ConversationDao::load(participants, session_);
    if (convDao)
    {
        Log.w() << "startConversation() was called for a conversation that already exists. "
                   "See findConversation().";
    }
    else
    {
        convDao.reset(new Dao::ConversationDao(session_));
        convDao->setParticipants(participants);
        convDao->save();

        auto conv = std::make_shared<Conversation>(client_, participants);
        conversations_[conv->id()] = conv;

        client_->onConversationAdded(conv->id());
    }

    return convDao->id();
}

bool Conversations::removeConversation(id_type conversationId)
{
    kulloAssert(conversations_.count(conversationId));

    std::shared_ptr<Conversation> conv = conversations_[conversationId];
    if (!conv->messages().empty()) return false;
    conv->draft()->clear();
    conv->deletePermanently();

    conversations_.erase(conversationId);
    emit client_->conversationRemoved(conversationId);

    return true;
}

std::map<KulloAddress, std::shared_ptr<Participant>> Conversations::participants()
{
    // lazy loading of participants
    if (!participantsLoaded_)
    {
        auto allParticipantsResult = Dao::ParticipantDao::latestSenders(session_);
        while (auto dao = allParticipantsResult->next())
        {
            auto addr = dao->address();
            auto part = std::make_shared<Participant>(client_, addr, dao.release());
            participants_[part->address()] = part;
        }
        participantsLoaded_ = true;
    }

    return participants_;
}

void Conversations::onDraftModified(id_type conversationId)
{
    Log.d() << "Calling Conversations::onDraftModified() with conversationId " << conversationId;

    if (!conversationsLoaded_)
    {
        Log.w() << "Conversations not yet loaded from database.";
        return;
    }

    if (conversations_.count(conversationId))
    {
        auto conv = conversations_[conversationId];
        kulloAssert(conv);
        conv->onDraftModified();
    }
}

void Conversations::onParticipantAddedOrModified(const std::string &addressString)
{
    const auto address = KulloAddress(addressString);

    if (!participantsLoaded_) return;

    if (participants_.count(address))
    {
        // update the existing participant object
        auto part = participants_[address];
        kulloAssert(part);
        part->reload();
    }
    else
    {
        // insert a new participant
        participants_[address] = std::make_shared<Participant>(client_, address);
    }
}

void Conversations::onMessageAdded(id_type conversationId, id_type messageId)
{
    if (conversations_.count(conversationId))
    {
        auto conv = conversations_[conversationId];
        kulloAssert(conv);
        conv->onMessageAdded(messageId);
    }
    else
    {
        // Stop signal path here.
        // conversationAdded will load everything into the UI
    }
}

void Conversations::onMessageDeleted(id_type conversationId, id_type messageId)
{
    if (!conversationsLoaded_) return;

    if (conversations_.count(conversationId))
    {
        auto conv = conversations_[conversationId];
        kulloAssert(conv);
        conv->onMessageDeleted(messageId);
    }
}

void Conversations::onMessageModified(id_type conversationId, id_type messageId)
{
    if (!conversationsLoaded_) return;

    if (conversations_.count(conversationId))
    {
        auto conv = conversations_[conversationId];
        kulloAssert(conv);
        conv->onMessageModified(messageId);
    }
}

void Conversations::onMessageAttachmentsDownloaded(id_type conversationId, id_type messageId)
{
    if (!conversationsLoaded_) return;

    if (conversations_.count(conversationId))
    {
        auto conv = conversations_[conversationId];
        kulloAssert(conv);
        conv->onMessageAttachmentsDownloaded(messageId);
    }
}

void Conversations::onConversationModified(id_type conversationId)
{
    if (!conversationsLoaded_) return;

    if (conversations_.count(conversationId))
    {
        auto conv = conversations_[conversationId];
        kulloAssert(conv);
        conv->onConversationModified();
    }
}

void Conversations::onConversationAdded(id_type conversationId)
{
    if (!conversationsLoaded_) return;

    auto conv = std::make_shared<Conversation>(client_, conversationId);
    kulloAssert(conv);
    kulloAssert(conversationId == conv->id());
    conversations_[conversationId] = conv;

    client_->onConversationAdded(conversationId);
}

}
}
