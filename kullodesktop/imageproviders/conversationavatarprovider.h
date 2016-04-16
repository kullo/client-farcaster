/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>

#include "kullodesktop/imageproviders/abstractavatarprovider.h"

namespace KulloDesktop {
namespace Imageproviders {

class ConversationAvatarProvider : public AbstractAvatarProvider
{
public:
    ConversationAvatarProvider(Qml::ClientModel &clientModel);

protected:
    QPixmap drawAvatar(const QString &url, const QSize &renderSize) override;

private:
    QPixmap avatarForAddress(QString &participantAddress, const QSize &renderSize);
    QPixmap composeAvatar(const QStringList &participantsAddresses, const QSize &renderSize);
};

}
}
