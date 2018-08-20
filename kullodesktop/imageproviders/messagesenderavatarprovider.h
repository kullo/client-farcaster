/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include "kullodesktop/imageproviders/abstractavatarprovider.h"

namespace KulloDesktop {
namespace Imageproviders {

class MessageSenderAvatarProvider : public AbstractAvatarProvider
{
public:
    MessageSenderAvatarProvider(Qml::Inbox &inbox);

protected:
    virtual QPixmap drawAvatar(const QString &url, const QSize &renderSize) override;

private:
    Qml::Inbox &inbox_;
};

}
}
