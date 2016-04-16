/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "participantmodel.h"

#include <desktoputil/stlqt.h>
#include <kulloclient/api/Messages.h>
#include <kulloclient/crypto/hasher.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Qml {

ParticipantModel::ParticipantModel(QObject *parent)
    : QObject(parent)
{
    Log.f() << "Don't instantiate Participant in QML.";
}

ParticipantModel::ParticipantModel(
        const std::shared_ptr<Kullo::Api::Session> &session,
        Kullo::id_type msgId,
        QObject *parent)
    : QObject(parent)
    , session_(session)
    , msgId_(msgId)
{
    kulloAssert(session_);
    kulloAssert(msgId >= 0);
}

QString ParticipantModel::name() const
{
    return QString::fromStdString(session_->senders()->name(msgId_));
}

QString ParticipantModel::address() const
{
    return QString::fromStdString(session_->senders()->address(msgId_)->toString());
}

QString ParticipantModel::organization() const
{
    return QString::fromStdString(session_->senders()->organization(msgId_));
}

QPixmap ParticipantModel::avatar() const
{
    auto mimeType = session_->senders()->avatarMimeType(msgId_);
    auto data = session_->senders()->avatar(msgId_);

    QByteArray type;
    if (mimeType == "image/png")  type = "png";
    if (mimeType == "image/jpeg") type = "jpeg";

    // Loading image data into a QPixmap takes a lot of time and is repeated
    // often with the same data, so let's cache it
    auto dataHash = Kullo::Crypto::Hasher::sha256(data);
    if (dataHash != avatarCacheDataHash_)
    {
        avatarCacheDataHash_ = dataHash;
        QByteArray avatarData = DesktopUtil::StlQt::toQByteArray(data);
        avatarCache_.loadFromData(avatarData, type.constData());
    }
    return avatarCache_;
}

}
}
