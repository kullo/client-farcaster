/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QString>
#include <QUrl>

namespace DesktopUtil {

class Filesystem
{
public:
    static QString prepareTmpFilename(const QString &original);
    static std::string shortenSourcePath(const std::string &filepath);
    static QUrl increaseFilenameCounter(const QUrl &file);
};

}
