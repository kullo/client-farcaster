/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "logenginefilewriter.h"

#include <csignal>
#include <exception>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <desktoputil/versions.h>
#include <QDir>
#include <QString>
#include <kulloclient/util/assert.h>

#include "kullodesktop/util/paths.h"

namespace KulloDesktop {
namespace Util {

using LibraryLogger = Kullo::Util::LibraryLogger;

std::unique_ptr<std::ofstream> LogEngineFileWriter::stream;
std::mutex LogEngineFileWriter::streamMutex;

LogFormat LogEngineFileWriter::logFormat_;

void LogEngineFileWriter::open()
{
    QDir().mkpath(logfileDir());
    auto dir = QDir(logfileDir());

    LogFormat HTML_FORMAT;
    HTML_FORMAT.filename = "kullo_log_%s.html";
    HTML_FORMAT.sessionHeader = "<p>&nbsp;</p><hr>\n<h2>Session %s</h2>\n<table border=\"1\">\n";
    HTML_FORMAT.sessionFooter = "</table>\n";
    HTML_FORMAT.rowFormat = "<tr><td>%s</td><td><small>%s</small></td><td>%s</td><td><small>%s</small></td></tr>";
    HTML_FORMAT.columns = { LogFormat::Type, LogFormat::Date, LogFormat::Message, LogFormat::CodePosition };
    HTML_FORMAT.dateFormat = "%Y-%m-%d %H:%M:%S";

//    LogFormat TXT_FORMAT;
//    TXT_FORMAT.filename = "kullo_log_%s.txt";
//    TXT_FORMAT.sessionHeader = "";
//    TXT_FORMAT.sessionFooter = "";
//    TXT_FORMAT.rowFormat = "%s [%s] %s";
//    TXT_FORMAT.columns = { LogFormat::Date, LogFormat::TypeShort, LogFormat::Message };
//    TXT_FORMAT.dateFormat = "%Y-%m-%d %H:%M:%s";
    LogEngineFileWriter::logFormat_ = HTML_FORMAT;

    auto filename = (boost::format(logFormat_.filename)
                     % DesktopUtil::Versions::kullodesktopVersion().toString()
                     ).str();

    QFileInfo fileInfo(dir, QString::fromStdString(filename));

    {
        std::lock_guard<std::mutex> lock(streamMutex); Q_UNUSED(lock)
        stream.reset(new std::ofstream(fileInfo.absoluteFilePath().toStdString(),
                                     std::ofstream::out | std::ofstream::app));
    }

    if (!logFormat_.sessionHeader.empty())
    {
        auto fmt = boost::format(logFormat_.sessionHeader)
                % getCurrentDateTime(logFormat_.dateFormat);
        log(fmt.str());
    }
}

void LogEngineFileWriter::close()
{
    std::lock_guard<std::mutex> lock(streamMutex); Q_UNUSED(lock)
    if (stream)
    {
        if (!logFormat_.sessionFooter.empty())
        {
            *stream << logFormat_.sessionFooter;
        }
        stream->close();
    }
}

void LogEngineFileWriter::libraryLoggerWrapper(
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

    std::string typeName;
    std::string typeShort;
    switch (type) {
    case LibraryLogger::LogType::Fatal:   typeName = "FATAL";   typeShort = "F"; break;
    case LibraryLogger::LogType::Error:   typeName = "ERROR";   typeShort = "E"; break;
    case LibraryLogger::LogType::Warning: typeName = "WARNING"; typeShort = "W"; break;
    case LibraryLogger::LogType::Info:    typeName = "INFO";    typeShort = "I"; break;
    case LibraryLogger::LogType::Debug:   typeName = "DEBUG";   typeShort = "D"; break;
    default: assert(false);
    }

    std::string position;
    if (!stacktrace.empty())
    {
        position = stacktrace;
    }
    else
    {
        position = file + ":" + std::to_string(line) + " (" + function + ")";
    }

    auto dateStr = getCurrentDateTime(logFormat_.dateFormat);

    // Create version dependent log file
    auto fmt = boost::format(logFormat_.rowFormat);
    for (auto type : logFormat_.columns)
    {
        switch (type) {
        case LogFormat::Type:
            fmt = fmt % typeName;
            break;
        case LogFormat::TypeShort:
            fmt = fmt % typeShort;
            break;
        case LogFormat::Date:
            fmt = fmt % dateStr;
            break;
        case LogFormat::Message:
            fmt = fmt % msg;
            break;
        case LogFormat::Thread:
            fmt = fmt % thread;
            break;
        case LogFormat::CodePosition:
            fmt = fmt % position;
            break;
        default:
            kulloAssert(false);
        }
    }

    log(fmt.str());

    if (type == LibraryLogger::LogType::Fatal)
    {
        std::terminate();
    }
}

void LogEngineFileWriter::log(const std::string txt)
{
    std::lock_guard<std::mutex> lock(streamMutex); Q_UNUSED(lock)
    *stream << txt << "\n";
    stream->flush();
}

std::string LogEngineFileWriter::getCurrentDateTime(std::string &format)
{
    auto datetime = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_facet * facet = new boost::posix_time::time_facet(format.c_str());
    std::ostringstream stream;
    stream.imbue(std::locale(stream.getloc(), facet));
    stream << datetime;
    return stream.str();
}

}
}
