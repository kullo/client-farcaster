/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QByteArray>
#include <QDateTime>
#include <kulloclient/api_impl/DateTime.h>

namespace DesktopUtil {

class KulloClient2Qt
{
public:
    static QDateTime toQDateTime(const Kullo::Api::DateTime &dt);

    // bin -> str conversion that does not convert to QString (UTF-16) first
    static std::string fromUtf8(const QByteArray &in);
};

}
