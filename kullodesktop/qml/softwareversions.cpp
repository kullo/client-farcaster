/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "softwareversions.h"

#include <desktoputil/versions.h>

namespace KulloDesktop {
namespace Qml {

SoftwareVersions::SoftwareVersions(QObject *parent)
    : QObject(parent)
{
}

QStringList SoftwareVersions::list()
{
    QStringList out;

    for (const auto &pair : DesktopUtil::Versions::all())
    {
        out << QString::fromStdString(pair.first + " " + pair.second);
    }

    return out;
}

QString SoftwareVersions::version()
{
    return QString::fromStdString(DesktopUtil::Versions::kullodesktopVersion().toString());
}

QString SoftwareVersions::versionSimplified()
{
    return QString::number(DesktopUtil::Versions::kullodesktopVersion().majorVersion());
}

}
}
