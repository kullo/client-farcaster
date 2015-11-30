/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "exceptionawareqapplication.h"

#include <exception>

#include <kulloclient/util/exceptions.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/stacktrace.h>

namespace KulloDesktop {
namespace Applications {

ExceptionAwareQApplication::ExceptionAwareQApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
}

bool ExceptionAwareQApplication::notify(QObject *receiver, QEvent *event)
{
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch (std::exception &ex)
    {
        Kullo::Util::Stacktrace::toStdErr();
        Log.f() << Kullo::Util::formatException(ex).c_str();
        return false;
    }
}

}
}
