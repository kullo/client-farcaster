/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <memory>
#include <vector>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/types.h>
#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/misc.h>

#include "desktoputil/dice/dice-forwards.h"

namespace Kullo {
namespace Model {

/**
 * @brief Describes a sender or recipient of a message.
 */
class Participant : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the participant's real name, e.g. "John Doe"
     * @brief The participant's real name
     */
    std::string name() const;

    /**
     * @brief Get the participant's Kullo address, e.g. "john.doe#kullo.net"
     * @brief The participant's Kullo address
     */
    Util::KulloAddress address() const;

    /**
     * @brief Get the participant's organization name, e.g. "Doe Inc."
     * @return The participant's organization, or an empty string
     */
    std::string organization() const;

    /**
     * @brief Get the MIME type of the participant's avatar, e.g. "image/jpeg"
     * @return A valid MIME type, or an empty string if no avatar is set
     */
    std::string avatarMimeType() const;

    /**
     * @brief Get the participant's avatar.
     * @return The contents of an image file, which is of the type returned by avatarMimeType(), or an empty array
     */
    std::vector<unsigned char> avatar() const;

    /**
     * @brief Loads a Participant from DB
     * @param query Must point at a valid record from which the data is loaded.
     * @param parent See QObject::parent()
     */
    Participant(Client *client, const Util::KulloAddress &address, Dao::ParticipantDao *dao, QObject *parent = 0);

    /**
     * @brief Loads a message sender from DB.
     *
     * @param messageId The message ID to be searched for
     * @param parent See QObject::parent()
     * @throw NotFoundException if no sender could be found
     */
    Participant(Client *client, id_type messageId, QObject *parent = 0);

    /**
     * @brief Loads the newest participant for a given address from DB.
     * @param address The address of the participant
     * @param parent See QObject::parent()
     * @throw NotFoundException if no sender could be found
     */
    Participant(Client *client, const Util::KulloAddress &address, QObject *parent = 0);

    ~Participant();

    /**
     * @brief Discards unsaved changes and reloads the data of this object.
     *
     * This is called when the syncer has modified the DB representation of
     * the participant.
     */
    void reload();

    void deletePermanently();

signals:
    /// Emitted when the participant was changed.
    void changed();

private:
    void newDao();

    Client *client_;
    std::unique_ptr<Dao::ParticipantDao> dao_;
    bool constructedForMessage_ = false;
    Util::KulloAddress address_;

    K_DISABLE_COPY(Participant);
};

}
}
