/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include "kullodesktop/imageproviders/abstractavatarprovider.h"

namespace KulloDesktop {
namespace Imageproviders {

class ParticipantAvatarProvider : public AbstractAvatarProvider
{
public:
    ParticipantAvatarProvider(Qml::Inbox &inbox);

protected:
    virtual QPixmap drawAvatar(const QString &url, const QSize &renderSize) override;

private:
    Qml::Inbox &inbox_;
};

}
}
