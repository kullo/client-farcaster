/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "qtypestreamers.h"

#include <ostream>
#include <QDebug>
#include <QDir>
#include <QPoint>
#include <QSize>
#include <QStringList>
#include <QUrl>
#include <QVariant>

namespace DesktopUtil {

QString toString(const QFileDevice::Permissions perm)
{
    auto out = QStringLiteral("------------");
    const std::vector<QFileDevice::Permission> perms = {
        QFileDevice::ReadOwner,
        QFileDevice::WriteOwner,
        QFileDevice::ExeOwner,
        QFileDevice::ReadUser,
        QFileDevice::WriteUser,
        QFileDevice::ExeUser,
        QFileDevice::ReadGroup,
        QFileDevice::WriteGroup,
        QFileDevice::ExeGroup,
        QFileDevice::ReadOther,
        QFileDevice::WriteOther,
        QFileDevice::ExeOther
    };
    for (int i = 0; i < 12; ++i)
    {
        if (perm & perms[i])
        {
            if (i%3 == 0) out[i] = QChar('r');
            if (i%3 == 1) out[i] = QChar('w');
            if (i%3 == 2) out[i] = QChar('x');
        }
    }
    return out;
}

}

std::ostream &operator<<(std::ostream &out, const QChar &c)
{
    if (c.isNull())
    {
        out << "QChar::Null";
    }
    else
    {
        QString cStr(c);
        out << "'" << cStr.toStdString() << "'";
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const QString &s)
{
    out << "\"" << s.toStdString() << "\"";
    return out;
}

std::ostream &operator<<(std::ostream &out, const QStringList &l)
{
    if (l.empty())
    {
        out << "[]";
    }
    else
    {
        QString str = "[\"" + l.join("\", \"") + "\"]";
        out << str.toStdString();
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const QSize &s)
{
    out << s.width() << "x" << s.height();
    return out;
}

std::ostream &operator<<(std::ostream &out, const QPoint &s)
{
    out << "(" << s.x() << "," << s.y() << ")";
    return out;
}

std::ostream &operator<<(std::ostream &out, const QDir &dir)
{
    out << dir.path();
    return out;
}

std::ostream &operator<<(std::ostream &out, const QFileInfo &fi)
{
    out << "«" << fi.absoluteFilePath() << "»";
    return out;
}

std::ostream &operator<<(std::ostream &out, const QFileInfoList &fil)
{
    if (fil.empty())
    {
        out << "[]";
    }
    else
    {
        bool first = true;
        QString str = "[«";
        for (QFileInfo fi : fil)
        {
            if (!first) str.append("», «");
            else first = false;
            str.append(fi.absoluteFilePath());
        }
        str.append("»]");
        out << str.toStdString();
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const QUrl &url)
{
    out << "‹" << url.toString().toStdString() << "›";
    return out;
}

std::ostream &operator<<(std::ostream &out, const QVariant &var)
{
    QString output;
    QDebug(&output) << var;
    return out << output.trimmed().toStdString().c_str();
}
