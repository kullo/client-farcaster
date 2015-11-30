/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "libraryloggermodel.h"

#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Qml {

LibraryLoggerModel::LibraryLoggerModel(QObject *parent) :
    QObject(parent)
{
}

void LibraryLoggerModel::e(const QString &msg, const QString &stacktrace)
{
    Log.e(stacktrace.toStdString()) << msg.trimmed().toStdString().c_str();
}

void LibraryLoggerModel::w(const QString &msg, const QString &stacktrace)
{
    Log.w(stacktrace.toStdString()) << msg.trimmed().toStdString().c_str();
}

void LibraryLoggerModel::i(const QString &msg, const QString &stacktrace)
{
    Log.i(stacktrace.toStdString()) << msg.trimmed().toStdString().c_str();
}

void LibraryLoggerModel::d(const QString &msg, const QString &stacktrace)
{
    Log.d(stacktrace.toStdString()) << msg.trimmed().toStdString().c_str();
}

}
}
