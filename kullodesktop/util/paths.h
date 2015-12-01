/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QString>

namespace KulloDesktop {
namespace Util {

QString logfileDir();

struct Paths {
    Paths();
    const QString KULLO_DIR;
    const QString UPDATES_DIR;
    const QString DATABASE_DIR;
    const QString BINARY_DIR;
    const QString CRASHREPORTER_ABSFILEPATH;
};

Paths& kulloPaths();

}
}
