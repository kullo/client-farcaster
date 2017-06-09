/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "attachmentpreviewprovider.h"

#include <desktoputil/qtypestreamers.h>
#include <desktoputil/stlqt.h>
#include <desktoputil/threadblocker.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api/DraftAttachments.h>
#include <kulloclient/api/DraftAttachmentsContentListener.h>
#include <kulloclient/api/MessageAttachments.h>
#include <kulloclient/api/MessageAttachmentsContentListener.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/qml/inbox.h"

namespace KulloDesktop {
namespace Imageproviders {

AttachmentPreviewProvider::AttachmentPreviewProvider(Qml::Inbox &inbox)
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
    , inbox_(inbox)
{
}

QPixmap AttachmentPreviewProvider::requestPixmap(const QString &url, QSize *size, const QSize &requestedSize)
{
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

    bool okConversationId;
    bool okMessageId;
    bool okAttachmentId;
    auto conversationId = pathParts[1].toInt(&okConversationId);
    auto messageId      = pathParts[2].toInt(&okMessageId);
    auto attachmentId   = pathParts[3].toInt(&okAttachmentId);
    if (!okConversationId || !okMessageId || !okAttachmentId || conversationId == -1)
    {
        Log.w() << "Invalid attachment preview string: " << url;
        return QPixmap();
    }
    bool isDraft = (messageId < 0);

    if (attachmentId == -1) return QPixmap(); // empty delegate

    std::shared_ptr<std::vector<uint8_t>> imageData = std::make_shared<std::vector<uint8_t>>();

    std::shared_ptr<Kullo::Api::AsyncTask> contentTask;
    std::shared_ptr<Kullo::Api::DraftAttachmentsContentListener> draftAttachmentContentListener;
    std::shared_ptr<Kullo::Api::MessageAttachmentsContentListener> messageAttachmentContentListener;

    if (isDraft)
    {
        class DraftAttachmentContentListener: public Kullo::Api::DraftAttachmentsContentListener {
        public:
            explicit DraftAttachmentContentListener(std::shared_ptr<std::vector<uint8_t>> target)
                : target_(target) {}

            void finished(int64_t, int64_t, const std::vector<uint8_t> & content) override {
                target_->assign(content.begin(), content.end());
            }
        private:
            std::shared_ptr<std::vector<uint8_t>> target_;
        };
        draftAttachmentContentListener = std::make_shared<DraftAttachmentContentListener>(imageData);
    }
    else
    {
        class MessageAttachmentsContentListener: public Kullo::Api::MessageAttachmentsContentListener {
        public:
            explicit MessageAttachmentsContentListener(std::shared_ptr<std::vector<uint8_t>> target)
                : target_(target) {}

            void finished(int64_t, int64_t, const std::vector<uint8_t> & content) override {
                target_->assign(content.begin(), content.end());
            }
        private:
            std::shared_ptr<std::vector<uint8_t>> target_;
        };
        messageAttachmentContentListener = std::make_shared<MessageAttachmentsContentListener>(imageData);
    }

    DesktopUtil::ThreadBlocker tb;
    Applications::KulloApplication::runOnMainThread([&](){
        auto session = inbox_.session();
        if (!session)
        {
            tb.release(false);
            return;
        }

        contentTask = isDraft
                ? session->draftAttachments()->contentAsync(conversationId, attachmentId, draftAttachmentContentListener)
                : session->messageAttachments()->contentAsync(messageId, attachmentId, messageAttachmentContentListener);

        tb.release(true);
    });

    auto success = tb.block();
    if (!success) return QPixmap();

    // done starting task

    contentTask->waitUntilDone();

    QImage image;
    bool ok = image.loadFromData(DesktopUtil::StlQt::toQByteArray(*imageData));
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
