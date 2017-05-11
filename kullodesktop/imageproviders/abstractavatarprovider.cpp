/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "abstractavatarprovider.h"

#include <QCryptographicHash>
#include <QPainter>
#include <cmath>

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

namespace {
const int RELATIVE_CORNER_RADIUS = 15; // %

const QColor FALLBACK_AVATAR_BACKGROUND_COLOR( 90,  90,  90);
const QColor FALLBACK_AVATAR_FOREGROUND_COLOR(255, 255, 255);
const qreal FALLBACK_AVATAR_FOREGROUND_OPACITY = 0.8;
}

namespace KulloDesktop {
namespace Imageproviders {

AbstractAvatarProvider::AbstractAvatarProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap AbstractAvatarProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QSize renderSize;
    if (requestedSize.isValid())
    {
        renderSize = requestedSize;
    }
    else
    {
        // Let image look ugly
        renderSize = QSize(32, 32);
        Log.w() << "Request image size not set for avatar. Use Image.sourceSize.";
    }

    /*
     * From QQuickImageProvider::requestPixmap:
     * > In all cases, size must be set to the original size of the image.
     * > This is used to set the width and height of the relevant Image if
     * > these values have not been set explicitly.
     *
     * Use default of 64x64 pixels as "original size" if requestedSize
     * not set.
     *
     */
    if (size)
    {
        size->setWidth(renderSize.width());
        size->setHeight(renderSize.height());
    }

    return drawAvatar(id, renderSize);
}

QPixmap AbstractAvatarProvider::getEmptyAvatar(const QSize &renderSize)
{
    const auto key = cacheKey(QStringLiteral("empty"), renderSize);

    QPixmap out;

    {
        std::lock_guard<std::mutex> lock(avatarsCacheMutex_); K_RAII(lock);

        if (!avatarsCache_.contains(key))
        {
            QImage image(QStringLiteral(":/resources/empty_avatar.png"));
            kulloAssert(!image.isNull());
            QImage resized = image.scaled(renderSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            avatarsCache_[key] = QPixmap::fromImage(resized);
        }

        out = avatarsCache_[key];
    }

    return out;
}

QPixmap AbstractAvatarProvider::getTextAvatar(const QString &text, const QSize &renderSize)
{
    const auto key = cacheKey(QStringLiteral("text=%1").arg(text), renderSize);

    QPixmap out;

    {
        std::lock_guard<std::mutex> lock(avatarsCacheMutex_); K_RAII(lock);

        if (!avatarsCache_.contains(key))
        {
            avatarsCache_[key] = makeTextAvatar(text, renderSize);
        }

        out = avatarsCache_[key];
    }

    return out;
}

QPixmap AbstractAvatarProvider::makeTextAvatar(const QString &text, const QSize &renderSize)
{
    QPixmap pixmap(renderSize);

    pixmap.fill(FALLBACK_AVATAR_BACKGROUND_COLOR);
    QPainter textPainter(&pixmap);
    QRect rect(0, 0, renderSize.width(), renderSize.height());
    int fontSize = std::lround(15.0 * renderSize.width() / 50.0);
    textPainter.setOpacity(FALLBACK_AVATAR_FOREGROUND_OPACITY);
    textPainter.setFont(QFont("Arial", fontSize, QFont::Bold));
    textPainter.setPen(QPen(FALLBACK_AVATAR_FOREGROUND_COLOR));
    textPainter.drawText(rect, Qt::AlignCenter, text);

    return pixmap;
}

QPixmap AbstractAvatarProvider::rounded(const QPixmap &in, bool cirlce)
{
    if (in.isNull()) return QPixmap();

    QImage out(in.width(), in.height(), QImage::Format_ARGB32);
    out.fill(Qt::transparent);

    QBrush brush(in);

    QPen pen;
    pen.setColor(Qt::transparent);
    pen.setJoinStyle(Qt::RoundJoin);

    QPainter painter(&out);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setBrush(brush);
    painter.setPen(pen);
    QRect bounds(0, 0, in.width(), in.height());

    const auto relativeCornerRadius = cirlce
            ? 100
            : RELATIVE_CORNER_RADIUS;

    painter.drawRoundedRect(bounds, relativeCornerRadius, relativeCornerRadius, Qt::RelativeSize);

    return QPixmap::fromImage(out);
}

QString AbstractAvatarProvider::cacheKey(const QString &contentIdentifier, const QSize &renderSize)
{
    return QStringLiteral("%1/%2/%3")
            .arg(contentIdentifier)
            .arg(renderSize.width())
            .arg(renderSize.height());
}

QString AbstractAvatarProvider::cacheKey(const QByteArray &data, const QSize &renderSize)
{
    const auto hash = QString::fromLatin1(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
    const auto contentIdentifier = QStringLiteral("raw=") + hash;
    return cacheKey(contentIdentifier, renderSize);
}

QPixmap AbstractAvatarProvider::decodeAvatar(const QByteArray &data, const QString &mimeType, const QSize &renderSize)
{
    QByteArray type;
    if (mimeType == "image/png")  type = "png";
    if (mimeType == "image/jpeg") type = "jpeg";

    QPixmap newPixmap;
    newPixmap.loadFromData(data, type.constData());

    if (!newPixmap.isNull()) {
        if (newPixmap.width() != newPixmap.height())
        {
            Log.w() << "Avatar must be square at this point "
                    << "(" << QSize(newPixmap.width(), newPixmap.height()) << ")";
        }

        newPixmap = newPixmap.scaled(renderSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return newPixmap;
}

}
}
