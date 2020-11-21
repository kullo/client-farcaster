/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "participantavatarprovider.h"

#include <QUrl>
#include <boost/optional.hpp>
#include <desktoputil/concurrency.h>
#include <desktoputil/initials.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api/AddressHelpers.h>
#include <kulloclient/api_impl/Address.h>
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

    auto nullableAdress = Kullo::Api::AddressHelpers::create(QUrl::fromPercentEncoding(path.toUtf8()).toStdString());
    if (!nullableAdress) {
        Log.e() << "Invalid Kullo address in avatar request string: " << url;
        return QPixmap();
    }
    auto address = *nullableAdress;

    boost::optional<QByteArray> participantAvatar;
    boost::optional<QString> participantAvatarMimeType;
    boost::optional<QString> participantName;

    std::promise<bool> successPromise;
    Applications::KulloApplication::runOnMainThread([&](){
        if (!inbox_.hasSession())
        {
            successPromise.set_value(false);
            return;
        }

        auto part = inbox_.latestSenderForAddress(address);
        if (part)
        {
            participantAvatar = part->avatar();
            participantAvatarMimeType = part->avatarMimeType();
            participantName = part->name();
        }

        successPromise.set_value(true);
    });
    if (!DesktopUtil::waitOrCrash(successPromise)) return QPixmap();

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
