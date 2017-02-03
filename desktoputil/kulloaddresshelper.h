/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
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
