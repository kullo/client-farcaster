/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "paths.h"

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <desktoputil/osdetection.h>
#include <kulloclient/util/assert.h>

namespace KulloDesktop {
namespace Util {

QString logfileDir()
{
    QString out;
#if defined K_OS_WINDOWS
    out = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Kullo";
#elif defined K_OS_OSX
    out = QDir::home().absolutePath() + "/.kullo";
#elif defined K_OS_LINUX
    out = QDir::home().absolutePath() + "/.kullo";
#endif
    return out;
}

Paths::Paths()
    #if defined K_OS_WINDOWS
        : KULLO_DIR(QStandardPaths::writableLocation(QStandardPaths::DataLocation))
        , UPDATES_DIR(QDir::home().absolutePath() + "/Documents/Kullo/Updates")
    #elif defined K_OS_OSX
        : KULLO_DIR  (QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QStringLiteral("/.kullo"))
        , UPDATES_DIR(QDir::home().absolutePath() + "/.kullo/updates")
    #elif defined K_OS_LINUX
        : KULLO_DIR  (QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QStringLiteral("/.kullo"))
        , UPDATES_DIR(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation))
    #endif

    #ifndef QT_NO_DEBUG
        , DATABASE_DIR(KULLO_DIR + "/debug")
    #else
        , DATABASE_DIR(KULLO_DIR)
    #endif
        , BINARY_DIR(QCoreApplication::applicationDirPath())
        , CRASHREPORTER_ABSFILEPATH(QDir(BINARY_DIR).absoluteFilePath("crashreporter"))
{
    /*
     * On Windows QStandardPaths needs QApplication to be initialized
     *
     * KULLO_DIR = C:\Users\kullo\AppData\Local\Kullo\KulloDebug\
     *                                          ^     ^
     *                               Organization     Application
     */
    kulloAssert(QCoreApplication::instance());
    kulloAssert(!QCoreApplication::organizationName().isEmpty());
    kulloAssert(!QCoreApplication::applicationName().isEmpty());
}

Paths& kulloPaths() {
    static Paths* ans = new Paths();
    return *ans;
}

}
}
