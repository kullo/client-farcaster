/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "participantavatarprovider.h"

#include <QUrl>
#include <boost/optional.hpp>
#include <desktoputil/initials.h>
#include <desktoputil/qtypestreamers.h>
#include <desktoputil/threadblocker.h>
#include <kulloclient/api/Address.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/sender.h"

namespace KulloDesktop {
namespace Imageproviders {

ParticipantAvatarProvider::ParticipantAvatarProvider(Qml::Inbox &inbox)
    : AbstractAvatarProvider()
    , inbox_(inbox)
{
}

QPixmap ParticipantAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    // Cut query string from URL
    QString path = url.split("?").at(0);

    std::shared_ptr<Kullo::Api::Address> address;
    try {
        auto addressString = QUrl::fromPercentEncoding(path.toUtf8());;
        address = Kullo::Api::Address::create(addressString.toStdString());
    }
    catch (std::invalid_argument)
    {
        Log.e() << "Invalid Kullo address in avatar request string: " << url;
        return QPixmap();
    }


    boost::optional<QByteArray> participantAvatar;
    boost::optional<QString> participantAvatarMimeType;
    boost::optional<QString> participantName;

    DesktopUtil::ThreadBlocker tb;

    Applications::KulloApplication::runOnMainThread([&](){
        if (!inbox_.hasSession())
        {
            tb.release(false);
            return;
        }

        auto part = inbox_.latestSenderForAddress(address);
        if (part)
        {
            participantAvatar = part->avatar();
            participantAvatarMimeType = part->avatarMimeType();
            participantName = part->name();
        }

        tb.release(true);
    });

    auto success = tb.block();
    if (!success) return QPixmap();

    QPixmap out;
    if (participantAvatar)
    {
        out = AbstractAvatarProvider::decodeAvatar(*participantAvatar, *participantAvatarMimeType, renderSize);

        if (out.isNull())
        {
            if (!participantName->isEmpty())
            {
                out = getTextAvatar(DesktopUtil::Initials::fromName(*participantName), renderSize);
            }
            else
            {
                out = getEmptyAvatar(renderSize);
            }
        }
    }
    else
    {
        out = getEmptyAvatar(renderSize);
    }

    return rounded(out);
}

}
}
