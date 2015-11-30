/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

namespace KulloDesktop {
namespace Qml {

class LibraryLoggerModel : public QObject
{
    Q_OBJECT

public:
    explicit LibraryLoggerModel(QObject *parent = 0);
    Q_INVOKABLE void e(const QString &msg, const QString &stacktrace = "");
    Q_INVOKABLE void w(const QString &msg, const QString &stacktrace = "");
    Q_INVOKABLE void i(const QString &msg, const QString &stacktrace = "");
    Q_INVOKABLE void d(const QString &msg, const QString &stacktrace = "");
};

}
}
