/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <fstream>
#include <memory>
#include <mutex>
#include <kulloclient/api/LogListener.h>

namespace KulloDesktop {
namespace Util {

class HtmlFileLogListener: public Kullo::Api::LogListener
{
public:
    HtmlFileLogListener();
    ~HtmlFileLogListener() override;

    void writeLogMessage(
            const std::string &file,
            int32_t line,
            const std::string &function,
            Kullo::Api::LogType type,
            const std::string &message,
            const std::string &thread,
            const std::string &stacktrace) override;

private:
    void log(const std::string &logLine);
    void openFile(const std::string &filename);
    void closeFile();

    std::mutex streamMutex_;
    std::unique_ptr<std::ofstream> stream_;
};

}
}
