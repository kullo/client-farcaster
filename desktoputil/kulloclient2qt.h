/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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
