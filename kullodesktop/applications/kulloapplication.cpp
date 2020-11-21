/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "kulloapplication.h"

#include <cstdlib>
#include <cstring>
#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Applications {

// http://www.guidgenerator.com/online-guid-generator.aspx
QString KulloApplication::GUID       = QStringLiteral("49de4518-8220-45cb-b196-6892c3ab89c8");
QString KulloApplication::GUID_DEBUG = QStringLiteral("ff702672-6763-469f-a255-df83466ef0ed");
bool KulloApplication::TEST_MODE = false;
bool KulloApplication::FAKE_LONG_MIGRATION = false;

KulloApplication::KulloApplication(int &argc, char **argv)
    : ExceptionAwareQApplication(argc, argv)
{
    connect(this, &KulloApplication::aboutToQuit, []() {
        Log.d() << "Time to say good bye!";
    });
}

void KulloApplication::runOnMainThread(const std::function<void ()> &function)
{
    QObject dummyObjectLivingInCallerThread;

    // qApp lives on main thread, so this slot will be called on main thread.
    // As this is an AutoConnection, code will run immediately if called from main thread.
    QObject::connect(&dummyObjectLivingInCallerThread, &QObject::destroyed, qApp, [function](QObject*) {
        function();
    });
}

}
}
