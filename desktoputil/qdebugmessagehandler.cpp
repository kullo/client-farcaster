/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "qdebugmessagehandler.h"

#include <vector>
#include <QString>
#include <kulloclient/util/librarylogger.h>

namespace DesktopUtil {

void QDebugMessageHandler::handler(QtMsgType type, const QMessageLogContext& context, const QString &message)
{
    std::string codePosition;

    if (context.category) codePosition += std::string("Category: ") + std::string(context.category) + " ";
    if (context.function) codePosition += std::string("Function: ") + std::string(context.function) + " ";
    if (context.file)     codePosition += std::string(context.file) + ":" + std::to_string(context.line);

    // Don't accept QML type warnings. Make them fatal
    //
    // "TypeError" cannot be fatal because we ocasionally get:
    // file:///C:/Qt-Installation/5.5/msvc2013/qml/QtQuick/Controls/Button.qml:96: TypeError: Cannot read property of null
    // file:///home/simon/nobackup/Qt/5.5/gcc_64/qml/QtQuick/Controls/Button.qml:96: TypeError: Cannot read property of null
    //
    // failed to load component:
    // > QQmlApplicationEngine failed to load component
    //
    // is not installed:
    // > qrc:/main.qml:3 module "QtQuick.Controls" is not installed qrc:/main.qml:4 module "QtQuick.Layouts" is not installed qrc:/main.qml:3 module "QtQuick.Controls" is not installed qrc:/main.qml:4 module "QtQuick.Layouts" is not installed
    if (type == QtWarningMsg)
    {
        auto fatalWarnings = std::vector<QString>{
                QStringLiteral("ReferenceError:"),
                QStringLiteral("is not a type"),
                QStringLiteral("File not found"),
                QStringLiteral("failed to load component"),
                QStringLiteral("is not installed"),
        };

        for (const auto &s : fatalWarnings)
        {
            if (message.contains(s))
            {
                type = QtFatalMsg;
                break;
            }
        }
    }

    switch (type) {
    case QtDebugMsg:    Log.d(codePosition) << message.trimmed().toStdString().c_str(); break;
    case QtWarningMsg:  Log.w(codePosition) << message.trimmed().toStdString().c_str(); break;
    case QtCriticalMsg: Log.e(codePosition) << message.trimmed().toStdString().c_str(); break;
    case QtFatalMsg:    Log.f(codePosition) << message.trimmed().toStdString().c_str(); break;
    case QtInfoMsg:     Log.i(codePosition) << message.trimmed().toStdString().c_str(); break;
    }
}

}
