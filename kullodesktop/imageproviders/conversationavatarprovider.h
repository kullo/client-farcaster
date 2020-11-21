/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <memory>

#include "kullodesktop/imageproviders/abstractavatarprovider.h"

namespace KulloDesktop {
namespace Imageproviders {

class ConversationAvatarProvider : public AbstractAvatarProvider
{
public:
    ConversationAvatarProvider(Qml::Inbox &inbox);

protected:
    QPixmap drawAvatar(const QString &url, const QSize &renderSize) override;

private:
    static QPixmap composeAvatar(const QList<QPixmap> participantAvatars, const QSize &renderSize);

    Qml::Inbox &inbox_;
};

}
}
