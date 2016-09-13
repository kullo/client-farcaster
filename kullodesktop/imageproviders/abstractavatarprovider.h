/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <cstdint>
#include <unordered_map>
#include <memory>

#include <QQuickImageProvider>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Imageproviders {

class AbstractAvatarProvider : public QQuickImageProvider
{
public:
    AbstractAvatarProvider(Qml::Inbox &inbox);
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

protected:
    virtual QPixmap drawAvatar(const QString &url, const QSize &renderSize) = 0;
    QPixmap getAvatarForAddress(const QString &address, const QSize &renderSize);
    QPixmap getFallbackAvatar(const QString name, const QSize &renderSize);
    QPixmap getEmptyAvatar(const QSize &renderSize);
    QPixmap rounded(const QPixmap &in);

    Qml::Inbox &inbox_;
    std::unordered_map<std::uint32_t, std::unique_ptr<QPixmap>> emptyAvatarCache_;
};

}
}
