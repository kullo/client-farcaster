/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "kullofoldersetup.h"

#include <QDir>
#include <QFileInfo>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Util {

void KulloFolderSetup::perpareKulloFolder(const QString &kulloFolderPath)
{
    QFileInfo kulloFolder(QDir(kulloFolderPath), "");
    if (!kulloFolder.exists())
    {
        Log.i() << "Creating Kullo folder: " << kulloFolderPath;
        if (!QDir().mkpath(kulloFolderPath))
        {
            Log.f() << "Could not create Kullo folder: " << kulloFolderPath;
        }
    }
    if (!kulloFolder.isWritable())
    {
        Log.f() << "Kullo folder not writeable: " << kulloFolderPath;
    }

#ifdef Q_OS_UNIX
    bool setPermissions = false;

    Log.d() << "Kullo folder current permissions: "
            << DesktopUtil::toString(QFile(kulloFolderPath).permissions());

    QFlags<QFileDevice::Permission> othersCanDoSomething = QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther;
    if (QFile(kulloFolderPath).permissions() & othersCanDoSomething)
    {
        Log.i() << "Kullo folder can be read, write or executed by others. Trying to set permissions for you ...";
        setPermissions = true;
    }

    QFlags<QFileDevice::Permission> groupCanDoSomething = QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ExeGroup;
    if (QFile(kulloFolderPath).permissions() & groupCanDoSomething)
    {
        Log.i() << "Kullo folder can be read, write or executed by group. Trying to set permissions for you ...";
        setPermissions = true;
    }

    if (setPermissions)
    {
        QFlags<QFileDevice::Permission> permissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner;
        if (!QFile(kulloFolderPath).setPermissions(permissions))
        {
            Log.w() << "Could not set permissions for Kullo folder: " << kulloFolderPath
                    << "Current permissions: " << DesktopUtil::toString(QFile(kulloFolderPath).permissions());
        }
        else
        {
            Log.i() << "Kullo folder permissions successfully set.";
        }
    }
#endif
}

}
}
