/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "usersettingsavatarprovider.h"

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/clientmodel.h"
#include "kullodesktop/qml/usersettingsmodel.h"

namespace KulloDesktop {
namespace Imageproviders {

UserSettingsAvatarProvider::UserSettingsAvatarProvider(Qml::ClientModel &clientModel)
    : AbstractAvatarProvider(clientModel)
{
}

QPixmap UserSettingsAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    auto userSettings = clientModel_.userSettings();

    // Cut query string from URL
    QString path = url.split("?").at(0);

    if (path != userSettings->address())
    {
        Log.w() << "Requested avatar url does not match user's Kullo address: " << url;
    }

    QPixmap out = getUserAvatarData(url, renderSize, userSettings);
    return out;
}

QPixmap UserSettingsAvatarProvider::getUserAvatarData(const QString &url, const QSize &renderSize, const Qml::UserSettingsModel *userSettings)
{
    QPixmap avatar;
    if (userSettings->tmpAvatarActive())
    {
        avatar = userSettings->tmpAvatar();
    }
    if (avatar.isNull())
    {
        avatar = userSettings->avatar();
    }

    QPixmap out;
    if (avatar.isNull())
    {
        out = getFallbackAvatar(userSettings->name(), renderSize);
    }
    else
    {
        if (avatar.width() != avatar.height())
        {
            Log.w() << "Avatar must be square at this point "
                    << "(" << QSize(avatar.width(), avatar.height()) << ")"
                    << ": " << url;
        }

        out = avatar.scaled(renderSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return out;
}

}
}
