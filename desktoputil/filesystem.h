/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QString>

namespace DesktopUtil {

class Filesystem
{
public:
    static QString prepareTmpFilename(const QString &original);
    static std::string shortenSourcePath(const std::string &filepath);
};

}
