/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "kullodesktop/util/logengineconsoleextended.h"

#include <iostream>
#include <mutex>
#include <boost/algorithm/string.hpp>
#include <kulloclient/util/misc.h>

namespace {
std::mutex mutex_;
}

namespace KulloDesktop {
namespace Util {

using LibraryLogger = Kullo::Util::LibraryLogger;

void LogEngineConsoleExtended::libraryLoggerWrapper(
        const std::string &file,
        int line,
        const std::string &function,
        LibraryLogger::LogType type,
        const std::string &msg,
        const std::string &thread,
        const std::string &stacktrace)
{
#ifdef NDEBUG
    // Ignore debug logs in release build
    if (type == LibraryLogger::LogType::Debug) return;
#endif

    std::string position;
    if (!stacktrace.empty())
    {
        position = stacktrace;
    }
    else
    {
        position = file + ":" + std::to_string(line) + " (" + function + ")";
    }

    auto message = msg;

    encodeLogMessage(message);
    encodeLogMessage(position);

    std::string logContent;
    switch (type) {
    case LibraryLogger::LogType::Fatal:   logContent = "[F]\t" + message + "\t" + thread + "\t" + position; break;
    case LibraryLogger::LogType::Error:   logContent = "[E]\t" + message + "\t" + thread + "\t" + position; break;
    case LibraryLogger::LogType::Warning: logContent = "[W]\t" + message + "\t" + thread + "\t" + position; break;
    case LibraryLogger::LogType::Info:    logContent = "[I]\t" + message + "\t" + thread + "\t" + position; break;
    case LibraryLogger::LogType::Debug:   logContent = "[D]\t" + message + "\t" + thread + "\t" + position; break;
    default: assert(false);
    }

    {
        std::lock_guard<std::mutex> lock(mutex_); K_UNUSED(lock);
        std::cerr << logContent << std::endl;
    }

    if (type == LibraryLogger::LogType::Fatal)
    {
        std::terminate();
    }
}

void LogEngineConsoleExtended::encodeLogMessage(std::string &msg)
{
    boost::replace_all(msg, "\n", "\\n");
    boost::replace_all(msg, "\t", "\\t");
}

}
}
