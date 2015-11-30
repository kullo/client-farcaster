/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "qdebugmessagehandler.h"

#include <vector>
#include <QString>
#include <kulloclient/util/librarylogger.h>
#include <desktoputil/osdetection.h>

namespace KulloDesktop {
namespace Util {

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
    if (type == QtWarningMsg)
    {
        auto fatalWarnings = std::vector<QString>{
                QStringLiteral("ReferenceError:"),
                QStringLiteral("is not a type"),
                QStringLiteral("File not found"),
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    case QtInfoMsg:     Log.i(codePosition) << message.trimmed().toStdString().c_str(); break;
#endif
    }
}

}
}
