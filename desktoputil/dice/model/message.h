/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <boost/optional.hpp>
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
 * @brief Describes a message
 *
 * Message represents only sent or received messages. Have a look at Draft for drafts.
 */
class Message : public QObject
{
    Q_OBJECT

public:
    /// Saves any state changes of the message to DB.
    void save();

    /**
     * @brief Marks the message as deleted.
     *
     * This might also delete the contents of the message, leaving a simple "tombstone"
     * in the DB for syncing.
     */
    void deletePermanently();

    /// Get the message ID as assigned by the server.
    id_type id() const;

    /// Get the conversation that this message is a part of.
    id_type conversationId() const;

    /**
     * @brief Get the modification timestamp as assigned by the server.
     *
     * The format is microseconds since 1970-01-01 00:00 UTC (i.e. timestamp * 1,000,000)
     */
    lastModified_type lastModified() const;

    /// Get the sender of this message.
    std::shared_ptr<Participant> sender();

    /**
     * @brief Get the message state.
     * @param state A state as defined in Message::MessageState, but not STATE_ANY
     * @return The current state value
     * @throw MessageStateException if no valid MessageState is passed
     */
    bool state(Dao::MessageState state) const;

    /**
     * @brief Set the message state.
     * @param state A state as defined in Message::MessageState, but not STATE_ANY
     * @param value The new state value
     * @throw MessageStateException if no valid MessageState is passed
     */
    void setState(Dao::MessageState state, bool value);

    /// Get the delivery state of the message.
    std::vector<Util::Delivery> deliveryState();

    /// Get the date and time of sending as set by the sending client.
    Util::DateTime dateSent() const;

    /// Get the date and time of receiving as set by the server.
    Util::DateTime dateReceived() const;

    /// Get the deleted status of the message.
    bool isDeleted() const;

    /// Get the main content part of the message.
    std::string text() const;

    /// Get the footer/imprint part of the message.
    std::string footer() const;

    /**
     * @brief Get the attachments for the message.
     * @return A potentially empty list of attachments.
     */
    std::vector<std::shared_ptr<Attachment>> attachments();

    /// Get the attachmets' downloded status.
    bool attachmentsReady();

    /**
     * @brief Compare messages by sent date
     * @return true iff this.dateSent() < other.dateSent()
     */
    bool operator<(const Message &other) const;

    /**
     * @brief Compare messages by sent date
     * @return true iff this.dateSent() > other.dateSent()
     */
    bool operator>(const Message &other) const;

    /**
     * @brief Loads a message from DB.
     *
     * @param messageId The message ID to be searched for
     * @param session The database session
     * @param parent See QObject::parent()
     * @throw NotFoundException if no message could be found
     */
    Message(Client *client, id_type messageId, QObject *parent = 0);

    /**
     * @brief Loads a Message from DB
     * @param session The database session
     * @param query Must point at a valid record from which the data is loaded.
     * @param parent See QObject::parent()
     */
    Message(Client *client, Dao::MessageDao *dao, QObject *parent = 0);

    ~Message();

    /**
     * @brief Discards unsaved state changes and reloads the state data of this object.
     *
     * This is called when the syncer has changed the DB representation of the message.
     */
    void reloadState();

    void onAttachmentsDownloaded();

signals:
    /// Emitted message was deleted permanently.
    void deleted(const Kullo::id_type messageId);

    /// Emitted when the state was changed.
    void stateChanged(const Kullo::id_type messageId);

    /// Emitted when the delivery information has been changed.
    void deliveryChanged(const Kullo::id_type messageId);

    /// Emitted when the attachments have been downloaded.
    void attachmentsDownloaded();

private:
    void loadDeliveryState();

    Client *client_ = nullptr;
    std::unique_ptr<Dao::MessageDao> dao_;
    boost::optional<std::vector<Util::Delivery>> deliveryState_;
    std::shared_ptr<Participant> sender_;
    std::vector<std::shared_ptr<Attachment>> attachments_;
    bool attachmentsLoadedFromDb_ = false;
    boost::optional<bool> attachmentsReady_;

    K_DISABLE_COPY(Message);
};

}
}
