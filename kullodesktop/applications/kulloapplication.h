/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <functional>

#include "kullodesktop/applications/exceptionawareqapplication.h"

namespace KulloDesktop {
namespace Applications {

class KulloApplication : public ExceptionAwareQApplication
{
    Q_OBJECT

public:
    explicit KulloApplication(int &argc, char **argv);

    static void runOnMainThread(const std::function<void()> &function);

    static QString GUID;
    static QString GUID_DEBUG;
    static bool TEST_MODE;
    static bool FAKE_LONG_MIGRATION;

signals:
    void showMainWindowRequested();
};

}
}
