/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include "kullodesktop/imageproviders/abstractavatarprovider.h"

namespace KulloDesktop {
namespace Imageproviders {

class ParticipantAvatarProvider : public AbstractAvatarProvider
{
public:
    ParticipantAvatarProvider(Qml::ClientModel &clientModel);

protected:
    virtual QPixmap drawAvatar(const QString &url, const QSize &renderSize);
};

}
}
