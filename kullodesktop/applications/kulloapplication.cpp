/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
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

}
}
