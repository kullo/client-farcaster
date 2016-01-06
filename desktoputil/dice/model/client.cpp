/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "client.h"

#include <kulloclient/util/assert.h>
#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/librarylogger.h>

#include "desktoputil/dice/model/conversation.h"
#include "desktoputil/dice/model/conversations.h"
#include "desktoputil/dice/sync/synccontroller.h"

using namespace Kullo::Db;
using namespace Kullo::Util;

namespace Kullo {
namespace Model {

const id_type Client::NOT_FOUND = std::numeric_limits<id_type>::max();

Client::Client(QObject *parent) :
    QObject(parent),
    syncController_(new Sync::SyncController(this))
{
    kulloAssert(parent == nullptr);

    connect(syncController_.get(), &Sync::SyncController::syncProgressed,
            this, &Client::syncProgressed);
    connect(syncController_.get(), &Sync::SyncController::syncFinished,
            this, &Client::syncFinished);
    connect(syncController_.get(), &Sync::SyncController::syncError,
            this, &Client::syncError);
    connect(syncController_.get(), &Sync::SyncController::draftAttachmentsTooBig,
            this, &Client::draftAttachmentsTooBig);
}

Client::~Client()
{
    logOut();
}

bool Client::loggedIn() const
{
    return session_ != nullptr;
}

void Client::prepareLogIn(const std::string &dbFile)
{
    dbFile_ = dbFile;
    session_ = makeSession(dbFile_);
    Db::migrate(session_);
    Log.i() << "Database session successfully migrated.";
}

void Client::logIn()
{
    kulloAssert(session_);
    logIn(nullptr);
}

void Client::logOut()
{
    syncController_->cancelSync();
    conversations_.reset();
    session_.reset();
    settings_.reset();
    dbFile_ = "";
}

bool Client::sync()
{
    return syncController_->sync(Sync::SyncMode::Everything);
}

std::map<id_type, std::shared_ptr<Conversation>> Client::conversations()
{
    kulloAssert(loggedIn());
    return conversations_->conversations();
}

id_type Client::findConversation(const std::set<KulloAddress> &participants)
{
    kulloAssert(loggedIn());

    return conversations_->findConversation(participants);
}

id_type Client::startConversation(const std::set<KulloAddress> &participants)
{
    kulloAssert(loggedIn());

    return conversations_->startConversation(participants);
}

bool Client::removeConversation(id_type conversationId)
{
    kulloAssert(loggedIn());
    return conversations_->removeConversation(conversationId);
}

UserSettings &Client::userSettings()
{
    return settings_;
}

std::map<KulloAddress, std::shared_ptr<Participant> > Client::participants() const
{
    kulloAssert(loggedIn());
    return conversations_->participants();
}

void Client::logIn(SharedSessionPtr session)
{
    if (session)
    {
        session_ = session;
    }

    kulloAssert(session_);
    kulloAssert(Db::hasCurrentSchema(session_));

    conversations_.reset(new Conversations(this, session_));
    connect(syncController_.get(), &Sync::SyncController::draftModified,
            conversations_.get(), &Conversations::onDraftModified);
    connect(syncController_.get(), &Sync::SyncController::participantAddedOrModified,
            conversations_.get(), &Conversations::onParticipantAddedOrModified);
    connect(syncController_.get(), &Sync::SyncController::messageAdded,
            conversations_.get(), &Conversations::onMessageAdded);
    connect(syncController_.get(), &Sync::SyncController::messageDeleted,
            conversations_.get(), &Conversations::onMessageDeleted);
    connect(syncController_.get(), &Sync::SyncController::messageModified,
            conversations_.get(), &Conversations::onMessageModified);
    connect(syncController_.get(), &Sync::SyncController::messageAttachmentsDownloaded,
            conversations_.get(), &Conversations::onMessageAttachmentsDownloaded);
    connect(syncController_.get(), &Sync::SyncController::conversationModified,
            conversations_.get(), &Conversations::onConversationModified);
    connect(syncController_.get(), &Sync::SyncController::conversationAdded,
            conversations_.get(), &Conversations::onConversationAdded);
}

std::string Client::dbFile() const
{
    kulloAssert(dbFile_ != "");
    return dbFile_;
}

SharedSessionPtr Client::session() const
{
    kulloAssert(loggedIn());
    return session_;
}

void Client::onConversationAdded(id_type conversationId)
{
    emit conversationAdded(conversationId);
}

}
}
