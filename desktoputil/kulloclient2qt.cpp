/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "kulloclient2qt.h"

namespace DesktopUtil {

QDateTime KulloClient2Qt::toQDateTime(const Kullo::Api::DateTime &dt)
{
    QDate date(dt.year, dt.month, dt.day);
    QTime time(dt.hour, dt.minute, dt.second);
    return QDateTime(date, time, Qt::OffsetFromUTC, dt.tzOffsetMinutes * 60);
}

std::string KulloClient2Qt::fromUtf8(const QByteArray &in)
{
    return std::string(in.cbegin(), in.cend());
}

}
