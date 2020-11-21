/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
