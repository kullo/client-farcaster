/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

namespace KulloDesktop {
namespace QmlComponents {

class LibraryLoggerQmlConnector : public QObject
{
    Q_OBJECT

public:
    explicit LibraryLoggerQmlConnector(QObject *parent = 0);
    Q_INVOKABLE void e(const QString &msg, const QString &stacktrace = "");
    Q_INVOKABLE void w(const QString &msg, const QString &stacktrace = "");
    Q_INVOKABLE void i(const QString &msg, const QString &stacktrace = "");
    Q_INVOKABLE void d(const QString &msg, const QString &stacktrace = "");
};

}
}
