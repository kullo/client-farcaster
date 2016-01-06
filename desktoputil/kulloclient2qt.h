/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QByteArray>
#include <QDateTime>
#include <kulloclient/util/datetime.h>

namespace DesktopUtil {

class KulloClient2Qt
{
public:
    static QDateTime toQDateTime(const Kullo::Util::DateTime &dt);
    static Kullo::Util::DateTime toDateTime(const QDateTime &qdt);

    // bin -> str conversion that does not convert to QString (UTF-16) first
    static std::string fromUtf8(const QByteArray &in);
};

}
