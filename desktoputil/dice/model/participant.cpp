/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "participant.h"

#include <kulloclient/dao/participantdao.h>
#include <kulloclient/util/assert.h>

#include "desktoputil/stlqt.h"
#include "desktoputil/dice/model/client.h"
#include "desktoputil/dice/model/exceptions.h"

using namespace Kullo::Util;

namespace Kullo {
namespace Model {

std::string Participant::name() const
{
    return dao_ ? dao_->name() : "";
}

KulloAddress Participant::address() const
{
    return address_;
}

std::string Participant::organization() const
{
    return dao_ ? dao_->organization() : "";
}

std::string Participant::avatarMimeType() const
{
    return dao_ ? dao_->avatarMimeType() : "";
}

std::vector<unsigned char> Participant::avatar() const
{
    return dao_ ? dao_->avatar() : std::vector<unsigned char>();
}

Participant::Participant(Client *client, const KulloAddress &address, Dao::ParticipantDao *dao, QObject *parent)
    : QObject(parent),
      client_(client),
      dao_(dao),
      address_(address)  // don't use dao->address() as dao might be null
{
    kulloAssert(parent == nullptr);
    kulloAssert(client_);
}

Participant::Participant(Client *client, id_type messageId, QObject *parent)
    : QObject(parent),
      client_(client),
      constructedForMessage_(true),
      address_(KulloAddress("will.be.overwritten#example.com"))
{
    kulloAssert(parent == nullptr);
    kulloAssert(client_);

    dao_ = Dao::ParticipantDao::load(messageId, client_->session());
    if (!dao_)
    {
        throw NotFound(std::string("Participant::Participant(): "
                                   "participant could not be found by messageId = ")
                       + std::to_string(messageId));
    }
    address_ = dao_->address();
}

Participant::Participant(Client *client, const KulloAddress &address, QObject *parent)
    : QObject(parent),
      client_(client),
      constructedForMessage_(false),
      address_(address)
{
    kulloAssert(parent == nullptr);
    kulloAssert(client_);

    newDao();
}

Participant::~Participant()
{
}

void Participant::reload()
{
    newDao();

    emit changed();
}

void Participant::deletePermanently()
{
    dao_->deletePermanently();
}

void Participant::newDao()
{
    kulloAssert(!constructedForMessage_);

    dao_ = Dao::ParticipantDao::load(address_, client_->session());
}

}
}
