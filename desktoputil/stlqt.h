/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <string>
#include <vector>
#include <QByteArray>
#include <QStringList>

namespace DesktopUtil {

class StlQt
{
public:
    static QByteArray toQByteArray(const std::vector<unsigned char> &vec);
    static QByteArray toQByteArray(const std::string &str);
    static std::vector<unsigned char> toVector(const QByteArray &qba);

    static QStringList toQStringList(const std::vector<std::string> &vec);
    static std::vector<std::string> toVector(const QStringList &qsl);
};

}
