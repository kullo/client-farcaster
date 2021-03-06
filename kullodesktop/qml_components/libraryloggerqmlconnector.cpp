/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "libraryloggerqmlconnector.h"

#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace QmlComponents {

LibraryLoggerQmlConnector::LibraryLoggerQmlConnector(QObject *parent) :
    QObject(parent)
{
}

void LibraryLoggerQmlConnector::e(const QString &msg, const QString &stacktrace)
{
    Log.e(stacktrace.toStdString()) << msg.trimmed().toStdString().c_str();
}

void LibraryLoggerQmlConnector::w(const QString &msg, const QString &stacktrace)
{
    Log.w(stacktrace.toStdString()) << msg.trimmed().toStdString().c_str();
}

void LibraryLoggerQmlConnector::i(const QString &msg, const QString &stacktrace)
{
    Log.i(stacktrace.toStdString()) << msg.trimmed().toStdString().c_str();
}

void LibraryLoggerQmlConnector::d(const QString &msg, const QString &stacktrace)
{
    Log.d(stacktrace.toStdString()) << msg.trimmed().toStdString().c_str();
}

}
}
