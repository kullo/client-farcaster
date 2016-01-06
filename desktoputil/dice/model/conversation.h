/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <set>
#include <QObject>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/types.h>
#include <kulloclient/dao/enums.h>
#include <kulloclient/util/datetime.h>
#include <kulloclient/util/misc.h>

#include "desktoputil/dice/dice-forwards.h"

namespace Kullo {
namespace Model {

/**
 * @brief Describes a conversation.
 *
 * A conversation consists of all messages sent between a fixed group of people.
 */
class Conversation : public QObject
{
    Q_OBJECT

public:
    static const Util::DateTime EMPTY_CONVERSATION_TIMESTAMP;

    /**
     * @brief Get the conversation ID.
     *
     * This ID is not synced and can thus differ between multiple clients
     * that access the same account.
     */
    id_type id() const;

    /**
     * @brief Get the group of people which participate in the conversation.
     * @return A map of the participants with the addresses as keys.
     *
     * This excludes the local user! If Alice, Bob and Cecilia have a
     * conversation and this client is Alice's client, participants() returns
     * only Bob and Cecilia.
     */
    std::map<Util::KulloAddress, std::shared_ptr<Participant>> participants();

    /**
     * @brief Counts all messages with the given message state.
     *
     * If anything other than STATE_ANY is passed, only the messages in the
     * given state are counted.
     */
    size_t countMessages(const Dao::MessageState state = Dao::MessageState::Any) const;

    /// Delete a conversation
    void deletePermanently();

    /// Get latest message's sent time
    Util::DateTime latestMessageTimestamp() const;

    /**
     * @brief Get the messages that belong to this conversation (both sent and received).
     * @return A map of the messages, using the messages' IDs as the keys.
     */
    std::map<id_type, std::shared_ptr<Message>> messages();

    /**
     * @brief Get the draft for the conversation.
     *
     * If the draft doesn't exist yet, a new one is created. A conversation
     * can only have a single draft.
     */
    std::shared_ptr<Draft> draft();

    /**
     * @brief Compare conversations by last incoming message's sent date
     * @return true iff this.lastIncomingMessageTime() < other.lastIncomingMessageTime()
     */
    bool operator<(const Conversation &other) const;

    /**
     * @brief Compare conversations by last incoming message's sent date
     * @return true iff this.lastIncomingMessageTime() > other.lastIncomingMessageTime()
     */
    bool operator>(const Conversation &other) const;

    /**
     * @brief Loads a Conversation from DB
     * @param query Must point at a valid record from which the data is loaded.
     * @param parent See QObject::parent()
     */
    Conversation(Client *client, Dao::ConversationDao *dao, QObject *parent = 0);

    /**
     * @brief Loads a conversation from DB, identified by its ID.
     * @param id The conversation ID to be searched for
     * @param parent See QObject::parent()
     * @throw NotFoundException if no conversation could be found
     */
    Conversation(Client *client, id_type id, QObject *parent = 0);

    /**
     * @brief Loads a conversation from DB, identified by its participants.
     * @param participants The participants of the conversation.
     * @param parent See QObject::parent()
     * @throw NotFoundException if no conversation could be found
     */
    Conversation(Client *client, std::set<Util::KulloAddress> participants, QObject *parent = 0);

    ~Conversation();

    void loadMessages();

    /**
     * @brief Handles modification of the draft.
     */
    void onDraftModified();

    /**
     * @brief Handles adding a new message.
     * @param messageId The ID of the message.
     */
    void onMessageAdded(id_type messageId);

    /**
     * @brief Handles deletion of one of the messages.
     * @param messageId The ID of the deleted message.
     */
    void onMessageDeleted(const id_type messageId);

    /**
     * @brief Handles state change of one of the messages.
     * @param messageId The ID of the deleted message.
     */
    void onMessageStateChanged(const id_type messageId);

    /**
     * @brief Handles modification of one of the messages.
     * @param messageId The ID of the modified message.
     */
    void onMessageModified(id_type messageId);

    /**
     * @brief Handles attachments downloaded of one of the messages.
     * @param messageId The ID of the modified message.
     */
    void onMessageAttachmentsDownloaded(id_type messageId);

    /**
     * @brief Handles modification of the conversation.
     */
    void onConversationModified();

    friend std::ostream &operator<<(std::ostream &out, const Conversation &conv);

signals:
    /**
     * @brief Emitted when the conversation was modified.
     *
     * This happens when for example a new message was received.
     */
    void modified();

    /// Emitted when a new message was added to the conversation.
    void messageAdded(Kullo::id_type messageId);

    /// Emitted when a message was deleted from the conversation.
    void messageDeleted(Kullo::id_type messageId);

    /// Emitted when the state was changed.
    void messageStateChanged(const Kullo::id_type messageId);

private:
    void addMessage(std::shared_ptr<Message> msg);
    void reloadLatestMessageTimestamp();
    void updateLatestMessageTimestamp(std::shared_ptr<Message> msg);
    void reloadMessageCounts();
    // Update counts and times
    // msg is either a new added message or a recently changed one
    void updateMeta(std::shared_ptr<Message> msg = nullptr);

    Client *client_ = nullptr;
    Util::DateTime latestMessageTimestamp_ = Util::DateTime::epoch();
    std::unique_ptr<Dao::ConversationDao> dao_;
    bool messagesLoaded_ = false;
    bool latestMessageTimestampLoaded_ = false;
    std::map<id_type, std::shared_ptr<Message>> messages_;
    std::shared_ptr<Draft> draft_;
    std::map<Dao::MessageState, std::size_t> messageCounts_;
    bool messageCountsLoaded_ = false;

    K_DISABLE_COPY(Conversation);
};

}
}
