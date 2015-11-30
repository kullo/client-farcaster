/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QString>

namespace DesktopUtil {

class Filesystem
{
public:
    static QString prepareTmpFilename(const QString &original);
};

}
