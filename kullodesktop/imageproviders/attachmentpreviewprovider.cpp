/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "attachmentpreviewprovider.h"

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/attachmentmodel.h"
#include "kullodesktop/qml/clientmodel.h"
#include "kullodesktop/qml/conversationlistmodel.h"
#include "kullodesktop/qml/conversationlistsource.h"
#include "kullodesktop/qml/conversationmodel.h"
#include "kullodesktop/qml/draftattachmentlistmodel.h"
#include "kullodesktop/qml/draftattachmentmodel.h"
#include "kullodesktop/qml/messagemodel.h"

namespace KulloDesktop {
namespace Imageproviders {

AttachmentPreviewProvider::AttachmentPreviewProvider(Qml::ClientModel &clientModel)
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
    , clientModel_(clientModel)
{
}

QPixmap AttachmentPreviewProvider::requestPixmap(const QString &url, QSize *size, const QSize &requestedSize)
{
    if (!clientModel_.loggedIn())
    {
        return QPixmap();
    }

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

    // Cut query string from URL
    QString path = url.split("?").at(0);

    //                      conversation    attachment
    //                                 v    v
    // Example path: simon%23kullo.net/6/35/1
    //               ^                   ^
    //               account             message
    QStringList pathParts = path.split('/');
    if (pathParts.size() != 4) return QPixmap();

    bool okConvId;
    bool okMsgId;
    bool okAttachmentPos;
    quint32 convId        = pathParts[1].toUInt(&okConvId);
    quint32 msgId         = pathParts[2].toUInt(&okMsgId);
    quint32 attachmentPos = pathParts[3].toUInt(&okAttachmentPos);
    if (!okConvId || !okMsgId || !okAttachmentPos || convId == 0)
    {
        Log.w() << "Invalid attachment preview string: " << url;
        return QPixmap();
    }

    QByteArray imageData;
    if (msgId == 0) // Draft
    {
        auto conversation = clientModel_.conversationsListSource()->get(convId);
        kulloAssert(conversation);
        auto draft = conversation->draft();
        kulloAssert(draft);
        auto attachment = draft->attachments()->get(attachmentPos);
        kulloAssert(attachment);
        imageData = attachment->content();
    }
    else
    {
        auto conversation = clientModel_.conversationsListSource()->get(convId);
        kulloAssert(conversation);
        auto message = conversation->messages()->get(msgId);
        kulloAssert(message);
        auto attachment = message->attachments()->get(attachmentPos);
        kulloAssert(attachment);
        imageData = attachment->content();
    }

    QImage image;
    bool ok = image.loadFromData(imageData);
    if (!ok)
    {
        Log.w() << "Could not load image data from attachment into image: " << url;
        return QPixmap();
    }

    return QPixmap::fromImage(image.scaled(renderSize,
                                           Qt::KeepAspectRatioByExpanding,
                                           Qt::SmoothTransformation));
}

}
}
