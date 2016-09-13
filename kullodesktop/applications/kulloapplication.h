/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>

#include "kullodesktop/applications/exceptionawareqapplication.h"

namespace KulloDesktop {
namespace Applications {

class KulloApplication : public ExceptionAwareQApplication
{
    Q_OBJECT

public:
    explicit KulloApplication(int &argc, char **argv);

    static QString GUID;
    static QString GUID_DEBUG;
    static bool TEST_MODE;
    static bool IGNORE_LOCK;
    static bool NO_TRAY_ICON;
    static bool FAKE_LONG_MIGRATION;

signals:
    void showMainWindowRequested();
};

}
}
