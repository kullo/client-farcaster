/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "sender.h"

#include <desktoputil/stlqt.h>
#include <kulloclient/api/Messages.h>
#include <kulloclient/api/Senders.h>
#include <kulloclient/api/Session.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Qml {

Sender::Sender(QObject *parent)
    : QObject(parent)
{
    Log.f() << "Don't instantiate Participant in QML.";
}

Sender::Sender(const std::shared_ptr<Kullo::Api::Session> &session,
               Kullo::id_type msgId,
               QObject *parent)
    : QObject(parent)
    , session_(session)
    , msgId_(msgId)
{
    kulloAssert(session_);
    kulloAssert(msgId >= 0);
}

QString Sender::name() const
{
    return QString::fromStdString(session_->senders()->name(msgId_));
}

QString Sender::address() const
{
    return QString::fromStdString(session_->senders()->address(msgId_)->toString());
}

QString Sender::organization() const
{
    return QString::fromStdString(session_->senders()->organization(msgId_));
}

QString Sender::avatarMimeType() const
{
    return QString::fromStdString(session_->senders()->avatarMimeType(msgId_));
}

QByteArray Sender::avatar() const
{
    return DesktopUtil::StlQt::toQByteArray(session_->senders()->avatar(msgId_));
}

}
}
