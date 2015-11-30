/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "messageavatarprovider.h"

#include <QBrush>
#include <QPainter>
#include <QPen>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/clientmodel.h"
#include "kullodesktop/qml/conversationlistmodel.h"
#include "kullodesktop/qml/conversationlistsource.h"
#include "kullodesktop/qml/conversationmodel.h"
#include "kullodesktop/qml/messagemodel.h"

namespace KulloDesktop {
namespace Imageproviders {

MessageAvatarProvider::MessageAvatarProvider(Qml::ClientModel &clientModel) :
    AbstractAvatarProvider(clientModel)
{
}

QPixmap MessageAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    if (!clientModel_.loggedIn())
    {
        return QPixmap();
    }

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
    quint32 convId = convIdAndMsgId[1].toUInt(&okConvId);
    quint32 msgId = convIdAndMsgId[2].toUInt(&okMsgId);
    if (!okConvId || !okMsgId || convId == 0 || msgId == 0)
    {
        Log.w() << "Invalid message avatar request string: " << url;
        return QPixmap();
    }

    Qml::ConversationModel *conv = clientModel_.conversationsListSource()->get(convId);
    if (!conv)
    {
        Log.e() << "Conversation for message avatar not found: " << convId;
        return QPixmap();
    }

    Qml::MessageModel *msg = conv->messages()->get(msgId);
    if (!msg)
    {
        Log.e() << "Message for avatar not found: " << msgId;
        return QPixmap();
    }

    QPixmap out;
    QPixmap avatar = msg->sender()->avatar();
    if (!avatar.isNull())
    {
        if (avatar.width() != avatar.height())
        {
            Log.w() << "Avatar must be square at this point "
                    << "(" << QSize(avatar.width(), avatar.height()) << ")"
                    << ": " << url;
        }

        out = avatar.scaled(renderSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        out = getFallbackAvatar(msg->sender()->name(), renderSize);
    }

    return rounded(out);
}

}
}
