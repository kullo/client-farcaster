/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include "kullodesktop/imageproviders/usersettingsavatarprovider.h"

namespace KulloDesktop {
namespace Imageproviders {

class SenderAvatarProvider : public UserSettingsAvatarProvider
{
public:
    SenderAvatarProvider(Qml::ClientModel &clientModel);

protected:
    virtual QPixmap drawAvatar(const QString &uri, const QSize &renderSize);
};

}
}
