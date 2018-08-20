/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <QString>
#include <QFileDevice>
#include <QFileInfo>

namespace DesktopUtil {
QString toString(const QFileDevice::Permissions perm);
}

class QDir;
class QPoint;
class QSize;
class QUrl;

std::ostream &operator<<(std::ostream &out, const QChar& c);
std::ostream &operator<<(std::ostream &out, const QString &s);
std::ostream &operator<<(std::ostream &out, const QStringList &l);
std::ostream &operator<<(std::ostream &out, const QSize &s);
std::ostream &operator<<(std::ostream &out, const QPoint &s);
std::ostream &operator<<(std::ostream &out, const QDir &dir);
std::ostream &operator<<(std::ostream &out, const QFileInfo &fi);
std::ostream &operator<<(std::ostream &out, const QFileInfoList &fil);
std::ostream &operator<<(std::ostream &out, const QUrl &url);
std::ostream &operator<<(std::ostream &out, const QVariant &var);
