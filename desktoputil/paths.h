/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QString>

namespace DesktopUtil {

struct Paths {
    Paths();
    const QString KULLO_DIR;
    const QString UPDATES_DIR;
    const QString DATABASE_DIR;
    const QString BINARY_DIR;
    const QString CRASHREPORTER_ABSFILEPATH;
};

Paths& kulloPaths();

// paths independent of QApplication
QString logfileDir();
QString defaultSaveAttachmentsDir();

}
