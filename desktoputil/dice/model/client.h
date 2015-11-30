/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <memory>
#include <set>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/types.h>
#include <kulloclient/db/dbsession.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/util/usersettings.h>

#include "desktoputil/dice/dice-forwards.h"

namespace Kullo {
namespace Model {

/**
 * @brief Implementation of the IClient interface
 */
class Client : public QObject
{
    Q_OBJECT

public:
    static const id_type NOT_FOUND;

    /**
     * @brief Constructs a new Client object
     * @param dbFile Path and filename for the SQLite DB file to be used
     * @param settings UserSettings instance to be used
     * @param parent see QObject::parent()
     * @throw DatabaseException if there's an error during DB initialization
     * @throw InvalidArgumentException if there's an error with the arguments
     */
    Client(QObject *parent = 0);

    ~Client();
    bool loggedIn() const;
    void prepareLogIn(const std::string &dbFile);
    void logIn();
    void logOut();

    /**
     * @brief Starts a new synchronization run.
     * @return false iff another sync is already running.
     *
     * Synchronization itself runs asynchronously, this method returns almost immediately.
     * Listen to the syncFinished() signal to get notified when the sync has finished.
     */
    bool sync();

    /**
     * @brief Get a map of all conversations that exist in the DB.
     * @return A map of all conversations with the conversation IDs as keys.
     */
    std::map<id_type, std::shared_ptr<Conversation>> conversations();

    /**
     * @brief Looks for a conversation given a set of participants.
     * @param participants Non-empty list of addresses of all intended recipients, excluding the local user.
     * @return The conversation's ID or IClient::NOT_FOUND when not found
     */
    id_type findConversation(const std::set<Util::KulloAddress> &participants);

    /**
     * @brief Starts a new conversation.
     * @param participants Non-empty list of addresses of all intended recipients, excluding the local user.
     * @return The new conversation's ID
     */
    id_type startConversation(const std::set<Util::KulloAddress> &participants);

    /// Remove an empty conversation.
    bool removeConversation(id_type conversationId);

    /// Returns the client's user settings.
    Util::UserSettings &userSettings();

    /// Get global list of participants
    std::map<Util::KulloAddress, std::shared_ptr<Participant>> participants() const;

    /**
     * @brief Login using an existing session.
     *
     * The session must be fully migrated! This interface is made for testing.
     *
     * @param session A fully migrated DB session.
     */
    void logIn(Db::SharedSessionPtr session);

    /// Returns the path including the filename of the database file.
    std::string dbFile() const;

    Db::SharedSessionPtr session() const;

    /**
     * @brief Handle/propagate added signal of a conversation.
     * @param conversationId The conversation the message belongs to.
     */
    void onConversationAdded(id_type conversationId);

signals:
    ///  Emitted when the sync has made progress
    void syncProgressed(const Sync::SyncProgress &progress);

    ///  Emitted when the sync process started by sync() has finished successfully.
    void syncFinished(const Sync::SyncProgress &progress);

    ///  Emitted when the sync process started by sync() has finished unsuccessfully.
    void syncError(std::exception_ptr exptr);

    /// Emitted when a new conversation has been created.
    void conversationAdded(Kullo::id_type conversationId);

    /// Emitted when a conversation has been removed.
    void conversationRemoved(Kullo::id_type conversationId);

    /// Emitted when syncer cannot send a message because its attachments are too big
    void draftAttachmentsTooBig(Kullo::id_type conversationId, std::size_t size, std::size_t sizeAllowed);

private:
    std::string dbFile_;
    Db::SharedSessionPtr session_;
    Util::UserSettings settings_;
    std::unique_ptr<Conversations> conversations_;
    std::unique_ptr<Sync::SyncController> syncController_;

    K_DISABLE_COPY(Client);
};

}
}
