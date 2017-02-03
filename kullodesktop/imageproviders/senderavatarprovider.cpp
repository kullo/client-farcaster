/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "senderavatarprovider.h"

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/usersettings.h"

namespace KulloDesktop {
namespace Imageproviders {

SenderAvatarProvider::SenderAvatarProvider(Qml::Inbox &inbox)
    : UserSettingsAvatarProvider(inbox)
{
}

QPixmap SenderAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    auto userSettings = inbox_.userSettings();

    // Cut query string from URL
    QString path = url.split("?").at(0);

    if (path != userSettings->address())
    {
        Log.w() << "Requested avatar url does not match user's Kullo address: " << url;
    }

    QPixmap out = getUserAvatarData(url, renderSize, userSettings);
    return rounded(out);
}

}
}
