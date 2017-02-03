/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "usersettingsavatarprovider.h"

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/usersettings.h"

namespace KulloDesktop {
namespace Imageproviders {

UserSettingsAvatarProvider::UserSettingsAvatarProvider(Qml::Inbox &inbox)
    : AbstractAvatarProvider(inbox)
{
}

QPixmap UserSettingsAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    auto userSettings = inbox_.userSettings();

    // Cut query string from URL
    QString path = url.split("?").at(0);

    if (path != userSettings->address())
    {
        Log.w() << "Requested avatar url does not match user's Kullo address: " << url;
    }

    QPixmap out = getUserAvatarData(url, renderSize, userSettings);
    return out;
}

QPixmap UserSettingsAvatarProvider::getUserAvatarData(const QString &url, const QSize &renderSize, const Qml::UserSettings *userSettings)
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
