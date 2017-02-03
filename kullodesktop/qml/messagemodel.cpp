/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "messagemodel.h"

#include <QQmlEngine>

#include <desktoputil/kulloclient2qt.h>
#include <kulloclient/api/Delivery.h>
#include <kulloclient/api/DeliveryState.h>
#include <kulloclient/api/MessageAttachments.h>
#include <kulloclient/api/Messages.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/util/strings.h>

namespace KulloDesktop {
namespace Qml {

MessageModel::MessageModel(QObject *parent)
    : QObject(parent)
{
    Log.f() << "Don't instantiate Message in QML.";
}

MessageModel::MessageModel(
        const std::shared_ptr<Kullo::Api::Session> &session,
        ApiMirror::EventDispatcher &eventDispatcher,
        Kullo::id_type msgId,
        QObject *parent)
    : QObject(parent)
    , session_(session)
    , msgId_(msgId)
    , sender_(session_, msgId_)
    , attachments_(session_, msgId_)
{
    kulloAssert(session_);

    connect(&eventDispatcher, &ApiMirror::EventDispatcher::messageAttachmentsDownloadedChanged,
            this, [this](Kullo::id_type conversationId, Kullo::id_type messageId)
    {
        K_UNUSED(conversationId);
        if (messageId == msgId_) onAttachmentsDownloaded();
    });
    connect(&eventDispatcher, &ApiMirror::EventDispatcher::messageStateChanged,
            this, [this](Kullo::id_type conversationId, Kullo::id_type messageId)
    {
        K_UNUSED(conversationId);
        if (messageId == msgId_) emit stateChanged(msgId_);
    });
    connect(&eventDispatcher, &ApiMirror::EventDispatcher::messageDeliveryChanged,
            this, [this](Kullo::id_type conversationId, Kullo::id_type messageId)
    {
        K_UNUSED(conversationId);
        if (messageId == msgId_) emit deliveryStatusChanged(msgId_);
    });
}

Kullo::id_type MessageModel::id() const
{
    return msgId_;
}

Kullo::id_type MessageModel::conversationId() const
{
    return session_->messages()->conversation(msgId_);
}

Sender *MessageModel::sender()
{
    QQmlEngine::setObjectOwnership(&sender_, QQmlEngine::CppOwnership);
    return &sender_;
}

bool MessageModel::read() const
{
    return session_->messages()->isRead(msgId_);
}

void MessageModel::setRead(bool value)
{
    session_->messages()->setRead(msgId_, value);
}

bool MessageModel::done() const
{
    return session_->messages()->isDone(msgId_);
}

void MessageModel::setDone(bool value)
{
    session_->messages()->setDone(msgId_, value);
}

QDateTime MessageModel::dateSent() const
{
    return DesktopUtil::KulloClient2Qt::toQDateTime(session_->messages()->dateSent(msgId_));
}

QDateTime MessageModel::dateReceived() const
{
    return DesktopUtil::KulloClient2Qt::toQDateTime(session_->messages()->dateReceived(msgId_));
}

QString MessageModel::text() const
{
    return QString::fromStdString(session_->messages()->text(msgId_));
}

QString MessageModel::textAsHtml() const
{
    std::string result = session_->messages()->text(msgId_);
    Kullo::Util::Strings::messageTextToHtml(result);
    return QString::fromStdString(result);
}

QString MessageModel::footer() const
{
    return QString::fromStdString(session_->messages()->footer(msgId_));
}

QString MessageModel::deliveryStatus() const
{
    auto deliveryState = session_->messages()->deliveryState(msgId_);
    if (deliveryState.empty())
    {
        return QStringLiteral("none");
    }
    else
    {
        auto something_failed = false;
        auto something_unsent = false;

        for (auto const &msgState : deliveryState)
        {
            using namespace Kullo::Api;
            auto state = msgState->state();
            something_failed = something_failed || (state == DeliveryState::Failed);
            something_unsent = something_unsent || (state == DeliveryState::Unsent);
        }

        if (something_failed)      return QStringLiteral("error");
        else if (something_unsent) return QStringLiteral("sending");
        else                       return QStringLiteral("sent");
    }
}

AttachmentListModel *MessageModel::attachments()
{
    QQmlEngine::setObjectOwnership(&attachments_, QQmlEngine::CppOwnership);
    return &attachments_;
}

bool MessageModel::attachmentsReady() const
{

    return session_->messageAttachments()->allAttachmentsDownloaded(msgId_);
}

void MessageModel::deletePermanently()
{
    session_->messages()->remove(msgId_);
}

bool MessageModel::operator<(const MessageModel &other) const
{
    return dateReceived() < other.dateReceived();
}

bool MessageModel::operator>(const MessageModel &other) const
{
    return dateReceived() > other.dateReceived();
}

void MessageModel::onAttachmentsDownloaded()
{
    Log.i() << "Attachments downloaded for message: " << id();
    emit attachmentsReadyChanged();
    emit attachmentsDownloaded(id());
}

}
}
