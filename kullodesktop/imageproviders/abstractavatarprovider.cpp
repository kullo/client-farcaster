/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "abstractavatarprovider.h"

#include <QPainter>
#include <cmath>

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/clientmodel.h"
#include "kullodesktop/qml/participantmodel.h"

namespace {
const int RELATIVE_CORNER_RADIUS = 15; // %
}

namespace KulloDesktop {
namespace Imageproviders {

AbstractAvatarProvider::AbstractAvatarProvider(Qml::ClientModel &clientModel)
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
    , clientModel_(clientModel)
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

QString AbstractAvatarProvider::initialsFromName(const QString &name)
{
    QStringList nameParts = name.split(' ', QString::SkipEmptyParts);
    QString initials;
    foreach (QString part, nameParts)
    {
        initials.append(part[0]);
    }
    return initials.left(3);
}

QPixmap AbstractAvatarProvider::getParticipantAvatar(const Qml::ParticipantModel &part, const QSize &renderSize)
{
    if (!part.avatar().isNull())
    {
        QPixmap avatar = part.avatar();
        if (avatar.width() != avatar.height())
        {
            Log.w() << "Participant avatar must be square at this point "
                    << "(" << QSize(avatar.width(), avatar.height()) << ")"
                    << ": " << part.address();
        }

        return avatar.scaled(renderSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        return getFallbackAvatar(part.name(), renderSize);
    }
}

QPixmap AbstractAvatarProvider::getFallbackAvatar(const QString name, const QSize &renderSize)
{
    QPixmap pixmap(renderSize);

    const QColor backgroundColor( 90,  90,  90);
    const QColor foregroundColor(200, 200, 200);
    pixmap.fill(backgroundColor);

    QPainter painter(&pixmap);
    QRect rect(0, 0, renderSize.width(), renderSize.height());
    int fontSize = std::lround(15.0 * renderSize.width() / 50.0);
    painter.setFont(QFont("Arial", fontSize, QFont::Bold));
    painter.setPen(QPen(foregroundColor));
    painter.drawText(rect, Qt::AlignCenter, initialsFromName(name));

    return pixmap;
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
