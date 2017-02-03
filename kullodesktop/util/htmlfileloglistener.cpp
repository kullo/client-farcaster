/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "kullodesktop/util/htmlfileloglistener.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <desktoputil/filesystem.h>
#include <desktoputil/paths.h>
#include <desktoputil/versions.h>
#include <kulloclient/api/LogType.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/util/strings.h>
#include <QDir>
#include <QFileInfo>
#include <QString>

namespace KulloDesktop {
namespace Util {

namespace {

const std::string FILENAME = "kullo_log_%s.html";
const std::string META = "<meta charset=\"UTF-8\">";
const std::string CSS = "<style>.f { color: rgb(192, 0, 0) } .e { color: rgb(255,64,64) } .w { color: rgb(255,128,0) } .i { color: rgb(16,16,16) } p { position: relative; padding-left: 11em; margin: 10px 0; } code { position: absolute; left: 0; top: 3px; } small { opacity: 0.2 } </style>";
const std::string SESSION_HEADER = META + "\n" + CSS + "\n" + "<p><strong>Starting new session %s</strong></p>";
const std::string ROW = "<p class=\"%s\"><code>%s %s</code> %s<br /><small>%s</small></p>";
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
    auto fmt = boost::format(SESSION_HEADER) % getCurrentDateTime();
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

    std::string typeCssClass;
    std::string typeName;
    switch (type) {
    case LogType::Fatal:   typeName = "F"; typeCssClass = "f"; break;
    case LogType::Error:   typeName = "E"; typeCssClass = "e"; break;
    case LogType::Warning: typeName = "W"; typeCssClass = "w"; break;
    case LogType::Info:    typeName = "I"; typeCssClass = "i"; break;
    case LogType::Debug:   typeName = "D"; typeCssClass = "d"; break;
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

    std::string messageHtml = Kullo::Util::Strings::htmlEscape(message);
    std::string positionHtml = Kullo::Util::Strings::htmlEscape(position);

    auto dateStr = getCurrentDateTime();
    auto fmt = boost::format(ROW) % typeCssClass % typeName % dateStr % messageHtml % positionHtml;
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
        stream_ = nullptr;
    }
}

}
}
