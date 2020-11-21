/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "paths.h"

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <kulloclient/util/assert.h>

#include "desktoputil/osdetection.h"

namespace DesktopUtil {

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
    static Paths instance;
    return instance;
}

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

QString defaultSaveAttachmentsDir()
{
    QString out;
    out = QDir::home().absolutePath();
    return out;
}

}
