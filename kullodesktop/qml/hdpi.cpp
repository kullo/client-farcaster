/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "hdpi.h"

#include <QScreen>

#include <desktoputil/osdetection.h>
#include <kulloclient/util/assert.h>

#include "kullodesktop/applications/kulloapplication.h"

namespace {
QScreen* firstScreen()
{
    const auto screens = KulloDesktop::Applications::KulloApplication::screens();
    kulloAssert(screens.size() >= 1);
    return screens[0];
}
}

namespace KulloDesktop {
namespace Qml {

Hdpi::Hdpi(QObject *parent) : QObject(parent)
{
}

qreal Hdpi::fontScalingFactor() const
{
    const auto screen = firstScreen();
    // Unfortunately, the value of logicalDotsPerInch does not change over the
    // lifetime of the application, so a running application will not be adjusted,
    // no matter which signals we send.
    const auto logicalDotsPerInch = screen->logicalDotsPerInch();

    qreal factor;
#ifdef K_OS_OSX
    factor = logicalDotsPerInch / 72.0;
#else
    factor = logicalDotsPerInch / 96.0;
#endif
    // Log.d() << "Screen logical dpi: " << logicalDotsPerInch;
    // Log.d() << "Factor: " << factor;
    return factor;
}

qreal Hdpi::maxDevicePixelRatio() const
{
    const auto screens = Applications::KulloApplication::screens();
    kulloAssert(screens.size() >= 1);

    qreal out = 0;
    for (auto screen: screens)
    {
        if (screen->devicePixelRatio() > out)
        {
            out = screen->devicePixelRatio();
        }
    }
    return out;
}

}
}
