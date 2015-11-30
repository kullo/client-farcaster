/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
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

}
}
