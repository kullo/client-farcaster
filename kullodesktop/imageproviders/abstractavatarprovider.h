/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <mutex>

#include <QQuickImageProvider>

#include <kulloclient/kulloclient-forwards.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Imageproviders {

class AbstractAvatarProvider : public QQuickImageProvider
{
public:
    AbstractAvatarProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

protected:
    virtual QPixmap drawAvatar(const QString &url, const QSize &renderSize) = 0;

    QPixmap getEmptyAvatar(const QSize &renderSize);
    QPixmap getTextAvatar(const QString &text, const QSize &renderSize);

    static QPixmap makeTextAvatar(const QString &text, const QSize &renderSize);
    static QPixmap rounded(const QPixmap &in, bool cirlce=false);
    static QString cacheKey(const QString &contentIdentifier, const QSize &renderSize);
    static QString cacheKey(const QByteArray &data, const QSize &renderSize);
    static QPixmap decodeAvatar(const QByteArray &data, const QString &mimeType, const QSize &renderSize);

    // Loading image data into a QPixmap takes a lot of time and is repeated
    // often with the same data, so let's cache it
    // This caches pixmaps for each size, that are not styled (roundes/cicle) and
    // not yet composed to group avatars
    QMap<QString, QPixmap> avatarsCache_;
    std::mutex avatarsCacheMutex_;
};

}
}
