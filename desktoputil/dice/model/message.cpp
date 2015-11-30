/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "message.h"

#include <smartsqlite/scopedtransaction.h>

#include <kulloclient/dao/attachmentdao.h>
#include <kulloclient/dao/deliverydao.h>
#include <kulloclient/dao/messagedao.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/datetime.h>

#include "desktoputil/stlqt.h"
#include "desktoputil/dice/model/attachment.h"
#include "desktoputil/dice/model/client.h"
#include "desktoputil/dice/model/exceptions.h"
#include "desktoputil/dice/model/participant.h"

using namespace Kullo::Dao;
using namespace Kullo::Util;

namespace Kullo {
namespace Model {

void Message::save()
{
    SmartSqlite::ScopedTransaction tx(client_->session(), SmartSqlite::Immediate);
    dao_->save(CreateOld::Yes);
    tx.commit();
}

void Message::deletePermanently()
{
    if (dao_->deleted()) return;

    const id_type messageId = id();

    // delete dependent objects
    for (auto attachment : attachments())
    {
        kulloAssert(attachment);
        attachment->deletePermanently();
    }
    sender();  // trigger lazy loading
    sender_->deletePermanently();

    dao_->clearData();
    dao_->setDeleted(true);

    SmartSqlite::ScopedTransaction tx(client_->session(), SmartSqlite::Immediate);
    dao_->save(CreateOld::Yes);
    tx.commit();

    emit deleted(messageId);
}

id_type Message::id() const
{
    return dao_->id();
}

id_type Message::conversationId() const
{
    return dao_->conversationId();
}

lastModified_type Message::lastModified() const
{
    return dao_->lastModified();
}

std::shared_ptr<Participant> Message::sender()
{
    // lazy loading
    if (!sender_)
    {
        sender_ = std::make_shared<Participant>(client_, dao_->id());
    }

    return sender_;
}

bool Message::state(MessageState state) const
{
    return dao_->state(state);
}

void Message::setState(MessageState state, bool value)
{
    // Trash button click deletes a message and
    // activates message => marked as read
    if (dao_->deleted()) return;

    if (dao_->setState(state, value))
    {
        emit stateChanged(id());
    }
}

std::vector<Delivery> Message::deliveryState()
{
    if (!deliveryState_)
    {
        loadDeliveryState();
    }
    return *deliveryState_;
}

DateTime Message::dateSent() const
{
    auto date = dao_->dateSent();
    if (date.empty()) return DateTime();
    return DateTime(date);
}

DateTime Message::dateReceived() const
{
    auto date = dao_->dateReceived();
    if (date.empty()) return DateTime();
    return DateTime(date);
}

bool Message::isDeleted() const
{
    return dao_->deleted();
}

std::string Message::text() const
{
    return dao_->text();
}

std::string Message::footer() const
{
    return dao_->footer();
}

std::vector<std::shared_ptr<Attachment> > Message::attachments()
{
    // lazy loading
    if (!attachmentsLoadedFromDb_)
    {
        auto result = AttachmentDao::allForMessage(IsDraft::No, dao_->id(), client_->session());
        while (auto dao = result->next())
        {
            attachments_.push_back(std::make_shared<Attachment>(dao.release(), nullptr));
        }
        attachmentsLoadedFromDb_ = true;
    }
    return attachments_;
}

bool Message::attachmentsReady()
{
    if (!attachmentsReady_)
    {
        attachmentsReady_ = AttachmentDao::allAttachmentsDownloaded(
                    dao_->id(), client_->session());
    }

    return attachmentsReady_.get();
}

bool Message::operator<(const Message &other) const
{
    return dateSent() < other.dateSent();
}

bool Message::operator>(const Message &other) const
{
    return dateSent() > other.dateSent();
}

Message::Message(Client *client, id_type messageId, QObject *parent)
    : QObject(parent),
      client_(client)
{
    kulloAssert(parent == nullptr);

    dao_ = MessageDao::load(messageId, Old::No, client_->session());
    if (!dao_)
    {
        throw NotFound("Message::Message(): "
                       "message could not be found by messageId");
    }

    loadDeliveryState();
}

Message::Message(Client *client, MessageDao *dao, QObject *parent)
    : QObject(parent)
    , client_(client)
    , dao_(dao)
{
    kulloAssert(parent == nullptr);
    kulloAssert(client_);
    kulloAssert(dao_);

    loadDeliveryState();
}

Message::~Message()
{
}

void Message::reloadState()
{
    // Reload state data
    // Attention: unsaved state changes are discarded
    id_type messageId = dao_->id();
    auto states = MessageDao::loadState(messageId, client_->session());
    for (auto iter : states)
    {
        auto key = iter.first;
        auto value = iter.second;
        setState(key, value);
    }

    loadDeliveryState();
}

void Message::onAttachmentsDownloaded()
{
    attachmentsReady_ = true;
    emit attachmentsDownloaded();
}

void Message::loadDeliveryState()
{
    auto old = std::move(deliveryState_);
    deliveryState_ = Dao::DeliveryDao(client_->session())
            .loadDelivery(dao_->id());

    if (old != deliveryState_)
    {
        emit deliveryChanged(id());
    }
}

}
}
