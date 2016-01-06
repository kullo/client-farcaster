/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "messagemodel.h"

#include <desktoputil/dice/model/message.h>

#include <desktoputil/kulloclient2qt.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/delivery.h>
#include <kulloclient/util/formatstring.h>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Qml {

MessageModel::MessageModel(QObject *parent)
    : QObject(parent)
{
    Log.e() << "Don't instantiate Message in QML.";
}

MessageModel::MessageModel(std::shared_ptr<Kullo::Model::Message> msg, QObject *parent)
    : QObject(parent)
    , msg_(msg)
    , sender_(msg->sender(), nullptr)
    , attachments_(msg, nullptr)
{
    kulloAssert(msg_);
    kulloAssert(parent == nullptr);

    connect(msg_.get(), &Kullo::Model::Message::attachmentsDownloaded, this, &MessageModel::onAttachmentsDownloaded);
    connect(msg_.get(), &Kullo::Model::Message::stateChanged,          this, &MessageModel::stateChanged);
    connect(msg_.get(), &Kullo::Model::Message::deliveryChanged,       this, &MessageModel::deliveryStatusChanged);
}

quint32 MessageModel::id() const
{
    return msg_->id();
}

quint32 MessageModel::conversationId() const
{
    return msg_->conversationId();
}

ParticipantModel *MessageModel::sender()
{
    return &sender_;
}

bool MessageModel::read() const
{
    return msg_->state(Kullo::Dao::MessageState::Read);
}

void MessageModel::setRead(bool value)
{
    msg_->setState(Kullo::Dao::MessageState::Read, value);
    msg_->save();
}

bool MessageModel::done() const
{
    return msg_->state(Kullo::Dao::MessageState::Done);
}

void MessageModel::setDone(bool value)
{
    msg_->setState(Kullo::Dao::MessageState::Done, value);
    msg_->save();
}

QDateTime MessageModel::dateSent() const
{
    return DesktopUtil::KulloClient2Qt::toQDateTime(msg_->dateSent());
}

QDateTime MessageModel::dateReceived() const
{
    return DesktopUtil::KulloClient2Qt::toQDateTime(msg_->dateReceived());
}

bool MessageModel::deleted() const
{
    return msg_->isDeleted();
}

QString MessageModel::text() const
{
    return QString::fromStdString(msg_->text());
}

QString MessageModel::textAsHtml() const
{
    std::string result = msg_->text();
    Kullo::Util::FormatString::messageTextToHtml(result);
    return QString::fromStdString(result);
}

QString MessageModel::footer() const
{
    return QString::fromStdString(msg_->footer());
}

QString MessageModel::deliveryStatus() const
{
    if (msg_->deliveryState().empty())
    {
        return QStringLiteral("none");
    }
    else
    {
        auto something_failed = bool{false};
        auto something_unsent = bool{false};

        for (auto const &msgState : msg_->deliveryState())
        {
            something_failed = something_failed || (msgState.state == Kullo::Util::Delivery::failed);
            something_unsent = something_unsent || (msgState.state == Kullo::Util::Delivery::unsent);
        }

        if (something_failed)      return QStringLiteral("error");
        else if (something_unsent) return QStringLiteral("sending");
        else                       return QStringLiteral("sent");
    }
}

AttachmentListModel *MessageModel::attachments()
{
    return &attachments_;
}

bool MessageModel::attachmentsReady() const
{

    return msg_->attachmentsReady();
}

void MessageModel::deletePermanently()
{
    msg_->deletePermanently();
}

bool MessageModel::operator<(const MessageModel &other) const
{
    return dateReceived() < other.dateReceived();
}

bool MessageModel::operator>(const MessageModel &other) const
{
    return dateReceived() > other.dateReceived();
}

void MessageModel::markAsReadAndDone()
{
    msg_->setState(Kullo::Dao::MessageState::Read, true);
    msg_->setState(Kullo::Dao::MessageState::Done, true);
    msg_->save();
}

void MessageModel::onAttachmentsDownloaded()
{
    Log.i() << "Attachments downloaded for message: " << id();
    emit attachmentsReadyChanged();
    emit attachmentsDownloaded(id());
}

}
}
