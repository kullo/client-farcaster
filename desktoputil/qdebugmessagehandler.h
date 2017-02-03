/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QMessageLogContext>

namespace DesktopUtil {

class QDebugMessageHandler
{
public:
    static void handler(QtMsgType type, const QMessageLogContext& context, const QString &message);
};

}
