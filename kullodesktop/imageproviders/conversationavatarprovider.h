/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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
