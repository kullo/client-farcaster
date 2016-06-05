/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "hdpi.h"

#include <QScreen>

#include <desktoputil/osdetection.h>
#include <kulloclient/util/assert.h>

#include "kullodesktop/applications/kulloapplication.h"

namespace KulloDesktop {
namespace Qml {

Hdpi::Hdpi(QObject *parent) : QObject(parent)
{
}

qreal Hdpi::fontScalingFactor() const
{
    const auto screens = Applications::KulloApplication::screens();
    kulloAssert(screens.size() >= 1);
    const auto logicalDotsPerInch = screens[0]->logicalDotsPerInch();
    //Log.d() << "Screen logical dpi: " << logicalDotsPerInch;
    //Log.d() << "Factor: " << logicalDotsPerInch/96.0;
#ifdef K_OS_OSX
    return logicalDotsPerInch / 72.0;
#else
    return logicalDotsPerInch / 96.0;
#endif
}

}
}
