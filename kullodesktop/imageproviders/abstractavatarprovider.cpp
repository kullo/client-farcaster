/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "abstractavatarprovider.h"

#include <QPainter>
#include <cmath>

#include <desktoputil/initials.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/sender.h"

namespace {
const int RELATIVE_CORNER_RADIUS = 15; // %

const QColor FALLBACK_AVATAR_BACKGROUND_COLOR( 90,  90,  90);
const QColor FALLBACK_AVATAR_FOREGROUND_COLOR(255, 255, 255);
const qreal FALLBACK_AVATAR_FOREGROUND_OPACITY = 0.8;
}

namespace KulloDesktop {
namespace Imageproviders {

AbstractAvatarProvider::AbstractAvatarProvider(Qml::Inbox &inbox)
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
    , inbox_(inbox)
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

QPixmap AbstractAvatarProvider::getAvatarForAddress(const QString &address, const QSize &renderSize)
{
    auto part = inbox_.latestSenderForAddress(Kullo::Api::Address::create(address.toStdString()));
    if (!part)
    {
        return getEmptyAvatar(renderSize);
    }

    auto avatar = part->avatar();
    if (!avatar.isNull())
    {
        if (avatar.width() != avatar.height())
        {
            Log.w() << "Participant avatar must be square at this point "
                    << "(" << QSize(avatar.width(), avatar.height()) << ")"
                    << ": " << part->address();
        }

        return avatar.scaled(renderSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else if (!part->name().isEmpty())
    {
        return getFallbackAvatar(part->name(), renderSize);
    }
    else
    {
        return getEmptyAvatar(renderSize);
    }
}

QPixmap AbstractAvatarProvider::getFallbackAvatar(const QString name, const QSize &renderSize)
{
    QPixmap pixmap(renderSize);

    pixmap.fill(FALLBACK_AVATAR_BACKGROUND_COLOR);
    QPainter textPainter(&pixmap);
    QRect rect(0, 0, renderSize.width(), renderSize.height());
    int fontSize = std::lround(15.0 * renderSize.width() / 50.0);
    textPainter.setOpacity(FALLBACK_AVATAR_FOREGROUND_OPACITY);
    textPainter.setFont(QFont("Arial", fontSize, QFont::Bold));
    textPainter.setPen(QPen(FALLBACK_AVATAR_FOREGROUND_COLOR));
    textPainter.drawText(rect, Qt::AlignCenter, DesktopUtil::Initials::fromName(name));

    return pixmap;
}

QPixmap AbstractAvatarProvider::getEmptyAvatar(const QSize &renderSize)
{
    std::uint32_t key = (renderSize.width() << 16) | renderSize.height();

    if (!emptyAvatarCache_.count(key))
    {
        QImage image(QStringLiteral(":/resources/empty_avatar.png"));
        kulloAssert(!image.isNull());
        QImage resized = image.scaled(renderSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        emptyAvatarCache_.emplace(key, Kullo::make_unique<QPixmap>(QPixmap::fromImage(resized)));
    }

    return *emptyAvatarCache_[key];
}

QPixmap AbstractAvatarProvider::rounded(const QPixmap &in)
{
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
    painter.drawRoundedRect(bounds, RELATIVE_CORNER_RADIUS, RELATIVE_CORNER_RADIUS, Qt::RelativeSize);

    return QPixmap::fromImage(out);
}

}
}
