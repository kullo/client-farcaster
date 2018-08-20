/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "messagesenderavatarprovider.h"

#include <desktoputil/concurrency.h>
#include <desktoputil/initials.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/conversationlistsortedfiltered.h"
#include "kullodesktop/qml/conversationlistsource.h"
#include "kullodesktop/qml/conversationmodel.h"
#include "kullodesktop/qml/messagemodel.h"

namespace KulloDesktop {
namespace Imageproviders {

MessageSenderAvatarProvider::MessageSenderAvatarProvider(Qml::Inbox &inbox)
    : AbstractAvatarProvider()
    , inbox_(inbox)
{
}

QPixmap MessageSenderAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    // Cut query string from URL
    QString path = url.split("?").at(0);

    // Example request path: simon%23kullo.net/35/582
    QStringList convIdAndMsgId = path.split('/');
    if (convIdAndMsgId.size() != 3)
    {
        return QPixmap();
    }

    bool okConvId;
    bool okMsgId;
    auto convId = convIdAndMsgId[1].toInt(&okConvId);
    auto msgId = convIdAndMsgId[2].toInt(&okMsgId);
    if (!okConvId || !okMsgId || convId < 0 || msgId < 0)
    {
        Log.w() << "Invalid message avatar request string: " << url;
        return QPixmap();
    }

    // filled in main thread
    QString senderName;
    QString senderAvatarMimeType;
    QByteArray senderAvatar;

    std::promise<bool> successPromise;
    Applications::KulloApplication::runOnMainThread([&]() {
        if (!inbox_.hasSession())
        {
            successPromise.set_value(false);
            return;
        }

        Qml::ConversationModel *conv = inbox_.conversationsListSource()->get(convId);
        if (!conv)
        {
            Log.e() << "Conversation for message avatar not found: " << convId;
            successPromise.set_value(false);
            return;
        }

        Qml::MessageModel *msg = conv->messages()->get(msgId);
        if (!msg)
        {
            Log.e() << "Message for avatar not found: " << msgId;
            successPromise.set_value(false);
            return;
        }

        senderName = msg->sender()->name();
        senderAvatarMimeType = msg->sender()->avatarMimeType();
        senderAvatar = msg->sender()->avatar();
        successPromise.set_value(true);
    });
    if (!DesktopUtil::waitOrCrash(successPromise)) return QPixmap();

    const auto cacheKey = AbstractAvatarProvider::cacheKey(senderAvatar, renderSize);

    QPixmap out;

    {
        std::lock_guard<std::mutex> lock(avatarsCacheMutex_); K_RAII(lock);

        if (!avatarsCache_.contains(cacheKey))
        {
            avatarsCache_[cacheKey] = AbstractAvatarProvider::decodeAvatar(
                senderAvatar, senderAvatarMimeType, renderSize);
        }

        out = avatarsCache_[cacheKey];
    }


    if (out.isNull())
    {
        out = getTextAvatar(DesktopUtil::Initials::fromName(senderName), renderSize);
    }

    return rounded(out);
}

}
}
