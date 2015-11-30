/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

#include <kulloclient/util/librarylogger.h>

namespace KulloDesktop {
namespace Util {

struct LogFormat {
    enum Column {
        Type,
        TypeShort,
        Date,
        Message,
        Thread,
        CodePosition
    };

    std::string filename;
    std::string sessionHeader;
    std::string sessionFooter;
    std::string rowFormat;
    // http://www.boost.org/doc/libs/1_57_0/doc/html/date_time/date_time_io.html#date_time.format_flags
    std::string dateFormat;
    std::vector<Column> columns;
};

class LogEngineFileWriter
{
public:
    static void open();
    static void close();
    static void libraryLoggerWrapper(
            const std::string &file,
            int line,
            const std::string &function,
            const Kullo::Util::LibraryLogger::LogType type,
            const std::string &msg,
            const std::string &thread,
            const std::string &stacktrace);
    static LogFormat logFormat_;

private:
    static void log(const std::string txt);
    static std::string getCurrentDateTime(std::string &format);

    // Don't use Qt types in Logger to avoid recursive myMessageOutput() calls.
    static std::unique_ptr<std::ofstream> stream;
    static std::mutex streamMutex;
};

}
}
