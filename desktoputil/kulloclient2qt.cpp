/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "kulloclient2qt.h"

namespace DesktopUtil {

QDateTime KulloClient2Qt::toQDateTime(const Kullo::Util::DateTime &dt)
{
    if (dt.isNull())
    {
        return QDateTime();
    }
    else
    {
        QDate date(dt.year(), dt.month(), dt.day());
        QTime time(dt.hour(), dt.minute(), dt.second());
        return QDateTime(date, time, Qt::OffsetFromUTC, dt.tzOffset());
    }
}

Kullo::Util::DateTime KulloClient2Qt::toDateTime(const QDateTime &qdt)
{
    if (qdt.isNull())
    {
        return Kullo::Util::DateTime();
    }
    else
    {
        auto date = qdt.date();
        auto time = qdt.time();
        auto tzOffset = qdt.offsetFromUtc();
        return Kullo::Util::DateTime(
                    date.year(), date.month(), date.day(),
                    time.hour(), time.minute(), time.second(),
                    tzOffset);
    }
}

std::string KulloClient2Qt::fromUtf8(const QByteArray &in)
{
    return std::string(in.cbegin(), in.cend());
}

}
