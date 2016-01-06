/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Util {

class LogEngineConsoleExtended
{
public:
    static void libraryLoggerWrapper(
            const std::string &file,
            int line,
            const std::string &function,
            Kullo::Util::LibraryLogger::LogType type,
            const std::string &msg,
            const std::string &thread,
            const std::string &stacktrace);

private:
    static void encodeLogMessage(std::string &msg);
};

}
}
