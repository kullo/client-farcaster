/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>

#include "kullodesktop/applications/exceptionawareqapplication.h"
#include "kullodesktop/osintegration/kullotrayicon.h"
#include "kullodesktop/qml/clientmodel.h"
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
    void setMainWindow(QWindow *mainWindow);
    void setClientModel(Qml::ClientModel *clientModel);

    static QString GUID;
    static QString GUID_DEBUG;
    static bool TEST_MODE;
    static bool IGNORE_LOCK;
    static bool NO_TRAY_ICON;
    static bool FAKE_LONG_MIGRATION;

signals:
    void showMainWindowRequested();

private:
    void setCloseToTray(bool closeToTray);

    Qml::DeviceSettings settings_;
    QWindow *mainWindow_ = nullptr;
    Qml::ClientModel *clientModel_ = nullptr;
    std::unique_ptr<OsIntegration::KulloTrayIcon> icon_;
};

}
}
