/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "senderavatarprovider.h"

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/clientmodel.h"
#include "kullodesktop/qml/usersettingsmodel.h"

namespace KulloDesktop {
namespace Imageproviders {

SenderAvatarProvider::SenderAvatarProvider(Qml::ClientModel &clientModel)
    : UserSettingsAvatarProvider(clientModel)
{
}

QPixmap SenderAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    auto userSettings = clientModel_.userSettings();

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
