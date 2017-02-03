/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "kullodesktop/util/htmlfileloglistener.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <desktoputil/filesystem.h>
#include <desktoputil/paths.h>
#include <desktoputil/versions.h>
#include <kulloclient/api/LogType.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/misc.h>
#include <QDir>
#include <QFileInfo>
#include <QString>

namespace KulloDesktop {
namespace Util {

namespace {

const std::string FILENAME = "kullo_log_%s.html";
const std::string HEADER = "<p>&nbsp;</p><hr>\n<h2>Session %s</h2>\n<table border=\"1\">";
const std::string FOOTER = "</table>";
const std::string ROW = "<tr><td>%s</td><td><small>%s</small></td><td>%s</td><td><small>%s</small></td></tr>";
const std::string DATE_FORMAT = "%Y-%m-%d %H:%M:%S";

std::string getCurrentDateTime()
{
    auto nowLocal = boost::posix_time::microsec_clock::local_time();
    auto facet = new boost::posix_time::time_facet(DATE_FORMAT.c_str());
    std::ostringstream stream;
    stream.imbue(std::locale(stream.getloc(), facet));
    stream << nowLocal;
    return stream.str();
}

}

HtmlFileLogListener::HtmlFileLogListener()
{
    // create and get logfile dir
    QDir().mkpath(DesktopUtil::logfileDir());
    auto dir = QDir(DesktopUtil::logfileDir());

    // open logfile
    auto filename = (
                boost::format(FILENAME)
                % DesktopUtil::Versions::kullodesktopVersion().toString()
                ).str();
    QFileInfo fileInfo(dir, QString::fromStdString(filename));
    openFile(fileInfo.absoluteFilePath().toStdString());

    // write header
    auto fmt = boost::format(HEADER) % getCurrentDateTime();
    log(fmt.str());
}

HtmlFileLogListener::~HtmlFileLogListener()
{
    closeFile();
}

void HtmlFileLogListener::writeLogMessage(
        const std::string &file,
        int32_t line,
        const std::string &function,
        Kullo::Api::LogType type,
        const std::string &message,
        const std::string &thread,
        const std::string &stacktrace)
{
    using Kullo::Api::LogType;
    K_UNUSED(thread);

#ifdef NDEBUG
    // Ignore debug logs in release build
    if (type == LogType::Debug) return;
#endif

    std::string typeName;
    switch (type) {
    case LogType::Fatal:   typeName = "FATAL";   break;
    case LogType::Error:   typeName = "ERROR";   break;
    case LogType::Warning: typeName = "WARNING"; break;
    case LogType::Info:    typeName = "INFO";    break;
    case LogType::Debug:   typeName = "DEBUG";   break;
    default: kulloAssert(false);
    }

    std::string position;
    if (!stacktrace.empty())
    {
        position = stacktrace;
    }
    else
    {
        position = DesktopUtil::Filesystem::shortenSourcePath(file) + ":" + std::to_string(line) + " (" + function + ")";
    }

    auto dateStr = getCurrentDateTime();
    auto fmt = boost::format(ROW) % typeName % dateStr % message % position;
    log(fmt.str());

    if (type == LogType::Fatal)
    {
        closeFile();
        std::terminate();
    }
}

void HtmlFileLogListener::log(const std::string &logLine)
{
    std::lock_guard<std::mutex> lock(streamMutex_); Q_UNUSED(lock)
    if (stream_) *stream_ << logLine << std::endl;
}

void HtmlFileLogListener::openFile(const std::string &filename)
{
    std::lock_guard<std::mutex> lock(streamMutex_); Q_UNUSED(lock)
    stream_ = Kullo::make_unique<std::ofstream>(
                filename, std::ofstream::out | std::ofstream::app);
}

void HtmlFileLogListener::closeFile()
{
    std::lock_guard<std::mutex> lock(streamMutex_); Q_UNUSED(lock)
    if (stream_)
    {
        *stream_ << FOOTER << std::endl;
        stream_.reset();
    }
}

}
}
