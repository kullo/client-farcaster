/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QMessageLogContext>

namespace KulloDesktop {
namespace Util {

class QDebugMessageHandler
{
public:
    static void handler(QtMsgType type, const QMessageLogContext& context, const QString &message);
};

}
}
