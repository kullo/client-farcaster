/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "kulloapplication.h"

#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Applications {

// http://www.guidgenerator.com/online-guid-generator.aspx
QString KulloApplication::GUID       = QStringLiteral("49de4518-8220-45cb-b196-6892c3ab89c8");
QString KulloApplication::GUID_DEBUG = QStringLiteral("ff702672-6763-469f-a255-df83466ef0ed");
bool KulloApplication::TEST_MODE = false;
bool KulloApplication::IGNORE_LOCK = false;
bool KulloApplication::NO_TRAY_ICON = false;
bool KulloApplication::FAKE_LONG_MIGRATION = false;

KulloApplication::KulloApplication(int &argc, char **argv)
    : ExceptionAwareQApplication(argc, argv)
{
    connect(this, &KulloApplication::aboutToQuit, []() {
        Log.d() << "Time to say good bye!";
    });
}

void KulloApplication::secondStageSetup()
{
    kulloAssert(!organizationName().isEmpty());
    kulloAssert(!applicationName().isEmpty());

    if (NO_TRAY_ICON)
    {
        setQuitOnLastWindowClosed(true);
    }
    else
    {
        setQuitOnLastWindowClosed(!settings_.closeToTray());
        connect(&settings_, &Qml::DeviceSettings::closeToTrayChanged, [this](bool value) {
            this->setQuitOnLastWindowClosed(!value);
        });
    }
}

Qml::DeviceSettings& KulloApplication::deviceSettings()
{
    return settings_;
}

}
}
