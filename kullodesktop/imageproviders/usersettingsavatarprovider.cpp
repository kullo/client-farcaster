/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "usersettingsavatarprovider.h"

#include <desktoputil/concurrency.h>
#include <desktoputil/initials.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/applications/kulloapplication.h"
#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/usersettings.h"

namespace KulloDesktop {
namespace Imageproviders {

UserSettingsAvatarProvider::UserSettingsAvatarProvider(Qml::Inbox &inbox)
    : AbstractAvatarProvider()
    , inbox_(inbox)
{
}

QPixmap UserSettingsAvatarProvider::drawAvatar(const QString &url, const QSize &renderSize)
{
    // Cut query string from URL
    const QString path = url.split("?").at(0);
    const bool styleRounded = url.split("?").at(1).contains("rounded");
    const bool styleCircle = url.split("?").at(1).contains("circle");

    // filled in main thread
    QString userName;
    QPixmap userAvatar;

    std::promise<bool> successPromise;
    Applications::KulloApplication::runOnMainThread([&]() {
        auto userSettings = inbox_.userSettings();
        if (path != userSettings->address())
        {
            Log.w() << "Requested avatar url does not match user's Kullo address: " << url;
        }

        if (userSettings->tmpAvatarActive())
        {
            userAvatar = userSettings->tmpAvatar();
        }
        if (userAvatar.isNull())
        {
            userAvatar = userSettings->avatar();
        }

        userName = userSettings->name();

        successPromise.set_value(true);
    });
    if (!DesktopUtil::waitOrCrash(successPromise)) return QPixmap();

    QPixmap out;

    if (userAvatar.isNull())
    {
        auto text = DesktopUtil::Initials::fromName(userName);
        out = getTextAvatar(text, renderSize);
    }
    else
    {
        if (userAvatar.width() != userAvatar.height())
        {
            Log.w() << "Avatar must be square at this point "
                    << "(" << QSize(userAvatar.width(), userAvatar.height()) << ")"
                    << ": " << url;
        }

        out = userAvatar.scaled(renderSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    if (styleRounded)
    {
        return rounded(out);
    }
    else if (styleCircle)
    {
        return rounded(out, true);
    }
    else
    {
        return out;
    }
}

}
}
