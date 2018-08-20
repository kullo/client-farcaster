/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "kullodesktop/util/consoleextendedloglistener.h"

#include <boost/algorithm/string.hpp>
#include <desktoputil/filesystem.h>
#include <iostream>
#include <kulloclient/api/LogType.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/misc.h>

namespace KulloDesktop {
namespace Util {

namespace {

void encodeLogMessage(std::string &msg)
{
    boost::replace_all(msg, "\n", "\\n");
    boost::replace_all(msg, "\t", "\\t");
}

}

void ConsoleExtendedLogListener::writeLogMessage(
        const std::string &file,
        int32_t line,
        const std::string &function,
        Kullo::Api::LogType type,
        const std::string &message,
        const std::string &thread,
        const std::string &stacktrace)
{
    using Kullo::Api::LogType;

#ifdef NDEBUG
    // Ignore debug logs in release build
    if (type == LogType::Debug) return;
#endif

    std::string position;
    if (!stacktrace.empty())
    {
        position = stacktrace;
    }
    else
    {
        position = DesktopUtil::Filesystem::shortenSourcePath(file) + ":" + std::to_string(line) + " (" + function + ")";
    }

    auto encodedMsg = message;
    encodeLogMessage(encodedMsg);
    encodeLogMessage(position);

    std::string logContent;
    switch (type) {
    case LogType::Fatal:   logContent = "[F]\t" + encodedMsg + "\t" + thread + "\t" + position; break;
    case LogType::Error:   logContent = "[E]\t" + encodedMsg + "\t" + thread + "\t" + position; break;
    case LogType::Warning: logContent = "[W]\t" + encodedMsg + "\t" + thread + "\t" + position; break;
    case LogType::Info:    logContent = "[I]\t" + encodedMsg + "\t" + thread + "\t" + position; break;
    case LogType::Debug:   logContent = "[D]\t" + encodedMsg + "\t" + thread + "\t" + position; break;
    default: kulloAssert(false);
    }

    {
        std::lock_guard<std::mutex> lock(stderrMutex_); K_RAII(lock);
        std::cerr << logContent << std::endl;
    }

    if (type == LogType::Fatal)
    {
        std::terminate();
    }
}

}
}
