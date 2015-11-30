/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QQuickImageProvider>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Imageproviders {

class AbstractAvatarProvider : public QQuickImageProvider
{
public:
    AbstractAvatarProvider(Qml::ClientModel &clientModel);
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

protected:
    static QString initialsFromName(const QString &name);
    virtual QPixmap drawAvatar(const QString &url, const QSize &renderSize) = 0;
    QPixmap getParticipantAvatar(const Qml::ParticipantModel &part, const QSize &renderSize);
    QPixmap getFallbackAvatar(const QString name, const QSize &renderSize);
    QPixmap rounded(const QPixmap &in);

    Qml::ClientModel &clientModel_;
};

}
}
