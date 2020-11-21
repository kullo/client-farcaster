/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QString>

namespace DesktopUtil {

class KulloAddressHelper
{
public:
    static bool isValidKulloAddress(const QString &address);
    static bool isValidKulloDomain(const QString &domain);
    static bool kulloAddressEqual(const QString &lhs, const QString &rhs);
    static QStringList splitKulloAddress(const QString &address);
};

}
