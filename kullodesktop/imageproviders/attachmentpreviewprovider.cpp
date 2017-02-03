/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "attachmentpreviewprovider.h"

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/attachmentmodel.h"
#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/conversationlistmodel.h"
#include "kullodesktop/qml/conversationlistsource.h"
#include "kullodesktop/qml/conversationmodel.h"
#include "kullodesktop/qml/draftattachmentlistmodel.h"
#include "kullodesktop/qml/draftattachmentmodel.h"
#include "kullodesktop/qml/messagemodel.h"

namespace KulloDesktop {
namespace Imageproviders {

AttachmentPreviewProvider::AttachmentPreviewProvider(Qml::Inbox &inbox)
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
    , inbox_(inbox)
{
}

QPixmap AttachmentPreviewProvider::requestPixmap(const QString &url, QSize *size, const QSize &requestedSize)
{
    if (!inbox_.loggedIn())
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
        Log.w() << "Request image size not valid for attachmentpreview (" << renderSize
                << "). Use Image.sourceSize.";
        // Let image look ugly
        renderSize = QSize(16, 16);
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
    bool okAttachmentId;
    auto convId       = pathParts[1].toInt(&okConvId);
    auto msgId        = pathParts[2].toInt(&okMsgId);
    auto attachmentId = pathParts[3].toInt(&okAttachmentId);
    if (!okConvId || !okMsgId || !okAttachmentId || convId == -1)
    {
        Log.w() << "Invalid attachment preview string: " << url;
        return QPixmap();
    }

    if (attachmentId == -1) return QPixmap(); // empty delegate

    QByteArray imageData;
    if (msgId < 0) // Draft
    {
        auto conversation = inbox_.conversationsListSource()->get(convId);
        kulloAssert(conversation);
        auto draft = conversation->draft();
        kulloAssert(draft);
        auto attachment = draft->attachments()->get(attachmentId);
        kulloAssert(attachment);
        imageData = attachment->content();
    }
    else
    {
        auto conversation = inbox_.conversationsListSource()->get(convId);
        kulloAssert(conversation);
        auto message = conversation->messages()->get(msgId);
        kulloAssert(message);
        auto attachment = message->attachments()->get(attachmentId);
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

    const auto pixmap = QPixmap::fromImage(image.scaled(renderSize,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation));

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
    kulloAssert(size);
    // Real size after scaling. One side is equal to renderSize, one is less or equal than renderSize
    size->setWidth(pixmap.width());
    size->setHeight(pixmap.height());

    return pixmap;
}

}
}
