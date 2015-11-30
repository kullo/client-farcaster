/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "asynctask.h"

#include <kulloclient/util/exceptions.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/stacktrace.h>

namespace DesktopUtil {

void AsyncTask::runDetached(std::function<void()> func)
{
    run(func).detach();
}

std::thread AsyncTask::run(std::function<void ()> func)
{
    auto backgroundFunction = [] (std::function<void()> func)
    {
        try
        {
            func();
        }
        catch (std::exception &ex)
        {
            Kullo::Util::Stacktrace::toStdErr();
            Log.f() << "Exception caught in background thread: "
                    << Kullo::Util::formatException(ex).c_str();
            return;
        }
    };
    return std::thread{ std::bind(backgroundFunction, func) };
}

}
