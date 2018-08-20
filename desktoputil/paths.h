/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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
