/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include "kullodesktop/applications/exceptionawareqapplication.h"
#include "kullodesktop/qml/devicesettings.h"

namespace KulloDesktop {
namespace Applications {

class KulloApplication : public ExceptionAwareQApplication
{
    Q_OBJECT

public:
    explicit KulloApplication(int &argc, char **argv);

    /// Some setup steps that need application name set
    void secondStageSetup();

    Qml::DeviceSettings& deviceSettings();

    static QString GUID;
    static QString GUID_DEBUG;
    static bool TEST_MODE;
    static bool IGNORE_LOCK;
    static bool NO_TRAY_ICON;
    static bool FAKE_LONG_MIGRATION;

signals:
    void showMainWindowRequested();

private:
    Qml::DeviceSettings settings_;
};

}
}
