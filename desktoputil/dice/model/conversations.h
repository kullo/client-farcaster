/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/types.h>
#include <kulloclient/db/dbsession.h>

#include "desktoputil/dice/dice-forwards.h"
#include "desktoputil/dice/model/conversation.h"

namespace Kullo {
namespace Model {

class Conversations : public QObject
{
    Q_OBJECT

public:
    explicit Conversations(Client *client_, Db::SharedSessionPtr session, QObject *parent = 0);
    ~Conversations();
    std::map<id_type, std::shared_ptr<Conversation>> conversations();
    id_type findConversation(const std::set<Util::KulloAddress> &participants);
    id_type startConversation(const std::set<Util::KulloAddress> &participants);
    bool removeConversation(id_type conversationId);

    /**
     * @brief Returns all participants that are known to the client.
     * @return Map of the participants, with their addresses as keys.
     */
    std::map<Util::KulloAddress, std::shared_ptr<Participant>> participants();

public slots:
    /**
     * @brief Handle/propagate modification signal of a draft.
     * @param conversationId The conversation the draft belongs to.
     */
    void onDraftModified(id_type conversationId);

    /**
     * @brief Handle/propagate add or modification signal of a participant.
     * @param address The participant address.
     */
    void onParticipantAddedOrModified(const std::string &addressString);

    /**
     * @brief Handle/propagate added signal of a message.
     * @param conversationId The conversation the message belongs to.
     * @param messageId The ID of the message.
     */
    void onMessageAdded(id_type conversationId, id_type messageId);

    /**
     * @brief Handle/propagate deleted signal of a message.
     * @param conversationId The conversation the message belongs to.
     * @param messageId The ID of the message.
     */
    void onMessageDeleted(id_type conversationId, id_type messageId);

    /**
     * @brief Handle/propagate modification signal of a message.
     * @param conversationId The conversation the message belongs to.
     * @param messageId The ID of the modified message.
     */
    void onMessageModified(id_type conversationId, id_type messageId);

    void onMessageAttachmentsDownloaded(id_type conversationId, id_type messageId);

    /**
     * @brief Handle/propagate modification signal of a conversation.
     * @param conversationId The conversation the message belongs to.
     */
    void onConversationModified(id_type conversationId);

    /**
     * @brief Handle/propagate added signal of a conversation.
     * @param conversationId The conversation the message belongs to.
     */
    void onConversationAdded(id_type conversationId);

private:
    Client *client_ = nullptr;
    Db::SharedSessionPtr session_;
    std::map<id_type, std::shared_ptr<Conversation>> conversations_;
    bool conversationsLoaded_ = false;
    std::map<Util::KulloAddress, std::shared_ptr<Participant>> participants_;
    bool participantsLoaded_ = false;

    K_DISABLE_COPY(Conversations);
};

}
}
