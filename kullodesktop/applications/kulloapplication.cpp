/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "kulloapplication.h"

#include <cstdlib>
#include <cstring>
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
        setCloseToTray(false);
    }
    else
    {
        setCloseToTray(settings_.closeToTray());
        connect(&settings_, &Qml::DeviceSettings::closeToTrayChanged, [this](bool value)
        {
            this->setCloseToTray(value);
        });
    }
}

Qml::DeviceSettings& KulloApplication::deviceSettings()
{
    return settings_;
}

void KulloApplication::setMainWindow(QWindow *mainWindow)
{
    mainWindow_ = mainWindow;
}

void KulloApplication::setClientModel(Qml::ClientModel *clientModel)
{
    clientModel_ = clientModel;
}

namespace {
bool runningUnderGnome()
{
    auto desktop = std::getenv("DESKTOP_SESSION");
    const std::string prefix = "gnome";
    return desktop && !std::strncmp(desktop, prefix.data(), prefix.length());
}
}

void KulloApplication::setCloseToTray(bool closeToTray)
{
    setQuitOnLastWindowClosed(!closeToTray);

    if (!closeToTray && runningUnderGnome())
    {
        // Hide icon if we don't close to tray, because otherwise the app won't
        // quit when the last window is closed.
        icon_.reset();
    }
    else if (!icon_)
    {
        kulloAssert(mainWindow_);
        kulloAssert(clientModel_);
        icon_ = Kullo::make_unique<OsIntegration::KulloTrayIcon>(*this, *mainWindow_, *clientModel_);
        Log.i() << "Done building tray icon.";
    }
}

}
}
