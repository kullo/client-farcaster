/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "breakpadsetup.h"

#include <iostream>
#include <kulloclient/util/exceptions.h>
#include <kulloclient/util/librarylogger.h>

#include "desktoputil/osdetection.h"
#include "desktoputil/paths.h"
#include "desktoputil/versions.h"

#ifdef K_HAS_BREAKPAD
    #include <breakpadwrapper/breakpad_all.h>
#endif // K_HAS_BREAKPAD

namespace DesktopUtil {

namespace {
#ifdef K_HAS_BREAKPAD
    const auto QUOTE = std::string("\"");
    const auto WINDOWS_PRE_CMD = std::string("cmd /S /C \"");
    const auto WINDOWS_POST_CMD = std::string("\"");

    bool kulloTestMode_;
    std::string crashreporterCmd_;
#endif // K_HAS_BREAKPAD
}

void BreakpadSetup::setup(const bool kulloTestMode)
{
#ifdef K_HAS_BREAKPAD
    kulloTestMode_ = kulloTestMode;

    std::stringstream cmdStream;
    cmdStream << '"' << kulloPaths().CRASHREPORTER_ABSFILEPATH.toStdString() << '"'
              << " --upload_url https://kullo.kullo.net:8443/upload"
              << " --product_name KulloClient"
              << " --product_version "
              << Versions::kullodesktopVersion().toString()
              << " --client_guid abcde"
              << " --dump_path ";
    crashreporterCmd_ = cmdStream.str();

    auto breakpad = BreakpadWrapper::Breakpad::getInstance();
    breakpad->registerCallback(
                [](const char *path, void *context, bool succeeded) -> bool
    {
        (void)context;

        std::exception_ptr exPtr = std::current_exception();
        if (exPtr)
        {
            try
            {
                std::rethrow_exception(exPtr);
            }
            catch (std::exception &ex)
            {
                Log.e() << Kullo::Util::formatException(ex).c_str();
            }
        }

        if (succeeded && !kulloTestMode_)
        {
            // Example command looks like this:
            // "C:/Program Files (x86)/Go Agent/pipelines/installer-windows/installer-windows/tmp/SourceDir/crashreporter" --upload_url https://kullo.kullo.net:8443/upload --product_name KulloClient --product_version 0.21.3-prev --client_guid abcde --dump_path "C:\\Users\\kullo\\AppData\\Local\\Kullo\\Kullo\\82b99aec-6740-425e-acec-b50b5e1d7d83.dmp"
            // Windows needs some special treatment because it messes up with quotes.
            // See http://stackoverflow.com/questions/9964865/c-system-not-working-when-there-are-spaces-in-two-different-parameters

#ifdef QT_NO_DEBUG
            crashreporterCmd_ += QUOTE + path + QUOTE;
            std::cerr << "Trying to run: " << crashreporterCmd_ << std::endl;

#ifdef K_OS_WINDOWS
            std::system(std::string(WINDOWS_PRE_CMD + crashreporterCmd_ + WINDOWS_POST_CMD).c_str());
#else
            std::system(crashreporterCmd_.c_str());
#endif

#else
            std::cerr << "Debug build, dumped to " << path << std::endl;
#endif
        }

        // don't let the system crash reporter handle the crash
        return true;
    });

#endif // K_HAS_BREAKPAD
}

}
