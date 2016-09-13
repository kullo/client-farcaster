/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include "kullodesktop/imageproviders/usersettingsavatarprovider.h"

namespace KulloDesktop {
namespace Imageproviders {

class SenderAvatarProvider : public UserSettingsAvatarProvider
{
public:
    SenderAvatarProvider(Qml::Inbox &inbox);

protected:
    virtual QPixmap drawAvatar(const QString &uri, const QSize &renderSize);
};

}
}
