/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "conversationavatarprovider.h"

#include <QPainter>
#include <desktoputil/concurrency.h>
#include <desktoputil/initials.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api/AddressHelpers.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/conversationlistsortedfiltered.h"
#include "kullodesktop/qml/conversationlistsource.h"
#include "kullodesktop/qml/conversationmodel.h"
#include "kullodesktop/qml/sender.h"

namespace KulloDesktop {
namespace Imageproviders {

ConversationAvatarProvider::ConversationAvatarProvider(Qml::Inbox &inbox)
    : AbstractAvatarProvider()
    , inbox_(inbox)
{
}

QPixmap ConversationAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    // Cut query string from URL
    QString path = url.split("?").at(0);

    // Example request path: user%23example.com/444
    QStringList pathParts = path.split('/');
    if (pathParts.size() != 2) return QPixmap();

    bool okConvId;
    auto convId = pathParts[1].toInt(&okConvId);
    if (!okConvId || convId < 0)
    {
        Log.e() << "Invalid conversation avatar request string: " << url;
        return QPixmap();
    }

    QList<Kullo::Api::Address> participantsAddresses;
    QList<boost::optional<QByteArray>> participantAvatars;
    QList<boost::optional<QString>> participantAvatarMimeTypes;
    QList<boost::optional<QString>> participantNames;

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
            Log.e() << "Conversation for conversation avatar not found: " << convId;
            successPromise.set_value(false);
            return;
        }

        for (const auto &addressString : conv->participantsAddresses())
        {
            participantsAddresses.append(*Kullo::Api::AddressHelpers::create(addressString.toStdString()));
        }

        if (participantsAddresses.size() <= 4)
        {
            for (const auto &participantAddress : participantsAddresses)
            {
                auto part = inbox_.latestSenderForAddress(participantAddress);
                if (part)
                {
                    participantAvatars.append(part->avatar());
                    participantAvatarMimeTypes.append(part->avatarMimeType());
                    participantNames.append(part->name());
                }
                else
                {
                    participantAvatars.append(boost::none);
                    participantAvatarMimeTypes.append(boost::none);
                    participantNames.append(boost::none);
                }
            }
        }
        else
        {
            // too large for composition, don't read any data
        }

        successPromise.set_value(true);
    });
    if (!DesktopUtil::waitOrCrash(successPromise)) return QPixmap();

    QList<QPixmap> participantAvatarPixmaps;
    for (int pos = 0; pos < participantAvatars.length(); ++pos)
    {
        const auto &avatar = participantAvatars[pos];
        const auto &mimeType = participantAvatarMimeTypes[pos];
        const auto &name = participantNames[pos];
        if (avatar)
        {
            QPixmap pixmap = AbstractAvatarProvider::decodeAvatar(*avatar, *mimeType, renderSize);

            if (pixmap.isNull())
            {
                if (!name->isEmpty())
                {
                    pixmap = getTextAvatar(DesktopUtil::Initials::fromName(*name), renderSize);
                }
                else
                {
                    pixmap = getEmptyAvatar(renderSize);
                }
            }

            participantAvatarPixmaps.append(pixmap);
        }
        else
        {
            participantAvatarPixmaps.append(getEmptyAvatar(renderSize));
        }
    }


    QPixmap out;

    if (participantsAddresses.size() == 1)
    {
        out = participantAvatarPixmaps[0];
    }
    else if (participantsAddresses.size() <= 4)
    {
        out = ConversationAvatarProvider::composeAvatar(participantAvatarPixmaps, renderSize);
    }
    else
    {
        const QString textContent = QStringLiteral("%1+1").arg(participantsAddresses.size());
        out = getTextAvatar(textContent, renderSize);
    }

    return rounded(out);
}

QPixmap ConversationAvatarProvider::composeAvatar(const QList<QPixmap> participantAvatars, const QSize &renderSize)
{
    const auto len = participantAvatars.size();
    kulloAssert(len >= 2 && len <= 4);

    QPixmap out(renderSize);
    out.fill(Qt::transparent);
    QPainter painter(&out);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);

    if (len == 2)
    {
        //   |
        // 1 | 2
        //   |
        QPixmap pm1 = participantAvatars[0];
        QPixmap pm2 = participantAvatars[1];
        const auto target1 = QRectF(                   0, 0, renderSize.width()/2-1, renderSize.height());
        const auto target2 = QRectF(renderSize.width()/2, 0, renderSize.width()/2,   renderSize.height());
        const auto source1 = QRectF(pm1.width()/4, 0, pm1.width()/2, pm1.height());
        const auto source2 = QRectF(pm2.width()/4, 0, pm1.width()/2, pm2.height());
        painter.drawPixmap(target1, pm1, source1);
        painter.drawPixmap(target2, pm2, source2);
    }
    else if (len == 3)
    {
        //   | 2
        // 1 |--
        //   | 3
        QPixmap pm1 = participantAvatars[0];
        QPixmap pm2 = participantAvatars[1];
        QPixmap pm3 = participantAvatars[2];
        const auto target1 = QRectF(                   0,                     0, renderSize.width()/2-1, renderSize.height());
        const auto target2 = QRectF(renderSize.width()/2,                     0, renderSize.width()/2,   renderSize.height()/2-1);
        const auto target3 = QRectF(renderSize.width()/2, renderSize.height()/2, renderSize.width()/2,   renderSize.height()/2);
        const auto source1 = QRectF(pm1.width()/4, 0, pm1.width()/2, pm1.height());
        const auto source2 = QRectF(0, 0, pm2.width(), pm2.height());
        const auto source3 = QRectF(0, 0, pm3.width(), pm3.height());
        painter.drawPixmap(target1, pm1, source1);
        painter.drawPixmap(target2, pm2, source2);
        painter.drawPixmap(target3, pm3, source3);
    }
    else if (len == 4)
    {
        // 1 | 2
        // -----
        // 3 | 4
        QPixmap pm1 = participantAvatars[0];
        QPixmap pm2 = participantAvatars[1];
        QPixmap pm3 = participantAvatars[2];
        QPixmap pm4 = participantAvatars[3];
        const auto target1 = QRectF(                   0,                     0, renderSize.width()/2-1, renderSize.height()/2-1);
        const auto target2 = QRectF(renderSize.width()/2,                     0, renderSize.width()/2,   renderSize.height()/2-1);
        const auto target3 = QRectF(                   0, renderSize.height()/2, renderSize.width()/2-1, renderSize.height()/2);
        const auto target4 = QRectF(renderSize.width()/2, renderSize.height()/2, renderSize.width()/2,   renderSize.height()/2);
        const auto source1 = QRectF(0, 0, pm1.width(), pm1.height());
        const auto source2 = QRectF(0, 0, pm2.width(), pm2.height());
        const auto source3 = QRectF(0, 0, pm3.width(), pm3.height());
        const auto source4 = QRectF(0, 0, pm4.width(), pm4.height());
        painter.drawPixmap(target1, pm1, source1);
        painter.drawPixmap(target2, pm2, source2);
        painter.drawPixmap(target3, pm3, source3);
        painter.drawPixmap(target4, pm4, source4);
    }

    return out;
}

}
}
