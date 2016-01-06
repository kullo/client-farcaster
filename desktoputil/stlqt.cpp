/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "stlqt.h"

namespace DesktopUtil {

QByteArray StlQt::toQByteArray(const std::vector<unsigned char> &vec)
{
    return QByteArray(reinterpret_cast<const char*>(vec.data()), vec.size());
}

QByteArray StlQt::toQByteArray(const std::string &str)
{
    return QByteArray(str.data(), str.size());
}

std::vector<unsigned char> StlQt::toVector(const QByteArray &qba)
{
    return std::vector<unsigned char>(qba.cbegin(), qba.cend());
}

QStringList StlQt::toQStringList(const std::vector<std::string> &vec)
{
    QStringList qsl;
    std::transform(vec.cbegin(), vec.cend(), std::back_inserter(qsl), QString::fromStdString);
    return qsl;
}

std::vector<std::string> StlQt::toVector(const QStringList &qsl)
{
    std::vector<std::string> vec;
    std::transform(qsl.cbegin(), qsl.cend(), std::back_inserter(vec),
                   [](const QString &qstr){return qstr.toStdString();});
    return vec;
}

}
