/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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
