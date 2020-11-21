/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

namespace KulloDesktop {
namespace QmlComponents {

class LibraryLoggerQmlConnector : public QObject
{
    Q_OBJECT

public:
    explicit LibraryLoggerQmlConnector(QObject *parent = nullptr);
    Q_INVOKABLE void e(const QString &msg, const QString &stacktrace = "");
    Q_INVOKABLE void w(const QString &msg, const QString &stacktrace = "");
    Q_INVOKABLE void i(const QString &msg, const QString &stacktrace = "");
    Q_INVOKABLE void d(const QString &msg, const QString &stacktrace = "");
};

}
}
