/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "programoptions.h"

#include <iostream>

#include <boost/program_options.hpp>
#include <QTimer>

#include <kulloclient/crypto/info.h>
#include <kulloclient/util/crash.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/util/strings.h>

#include <desktoputil/osdetection.h>
#include <desktoputil/qtypestreamers.h>
#include <desktoputil/versions.h>
#include <kullodesktop/applications/kulloapplication.h>

namespace KulloDesktop {
namespace OsIntegration {

namespace po = boost::program_options;
int ProgramOptions::NA = 77777777;

namespace {
// User
const char* O_HELP         = "help";
const char* O_VERSION      = "version";

// Developer
const char* O_OS                  = "os";
const char* O_CRYPTO_INFO         = "crypto-info";
const char* O_VERSIONS            = "versions";
const char* O_LOG_FORMAT_EXTENDED = "logformat-extended";
const char* O_QT_BUILD_VERSION    = "qt-build-version";
const char* O_TEST                = "test";
const char* O_PATHS               = "paths";
const char* O_FAKE_LONG_MIGRATION = "fake-long-migration";
const char* O_CRASHTEST_NULLPTR   = "crashtest-nullptr";
const char* O_CRASHTEST_THROW     = "crashtest-throw";
// must not be changed, Qt Creator passes this argument when debugging QML
const char* O_QMLDEBUGGING        = "qmljsdebugger";
}

ProgramOptions::ProgramOptions()
    : vm_(Kullo::make_unique<po::variables_map>())
{
    optionsAll_ = Kullo::make_unique<po::options_description>("Options");
    optionsUser_ = Kullo::make_unique<po::options_description>("Options");
    optionsDeveloper_ = Kullo::make_unique<po::options_description>("Developer Options");

    optionsUser_->add_options()
            (O_HELP,         "Print help message")
            (O_VERSION,      "Show Kullo version and exit")
    ;

    optionsDeveloper_->add_options()
            (O_OS,                  "Show os information of this binary")
            (O_CRYPTO_INFO,         "Show available crypto implementations")
            (O_VERSIONS,            "Show versions of components")
            (O_LOG_FORMAT_EXTENDED, "Use extended log format")
            (O_QT_BUILD_VERSION,    "Qt versions Kullo was build with")
            (O_TEST,                "Test Kullo and close")
            (O_PATHS,               "Show some path information")
            (O_FAKE_LONG_MIGRATION, "Fake long running database migration")
            (O_CRASHTEST_NULLPTR,   "Let Kullo crash because of nullpointer dereference")
            (O_CRASHTEST_THROW,     "Let Kullo crash because uncaught exception")
            (O_QMLDEBUGGING, po::value<std::string>(), "Enable QML debugging")
    ;

    optionsAll_->add(*optionsUser_);
    optionsAll_->add(*optionsDeveloper_);
}

ProgramOptions::~ProgramOptions()
{
}

int ProgramOptions::parse(int argc, char *argv[])
{
    try {
        auto parserResult = po::parse_command_line(
                    argc, argv, *optionsAll_,
                    po::command_line_style::unix_style
                    // this is necessary for QML debugging ("-qmljsdebugger" with only one dash)
                    | po::command_line_style::allow_long_disguise);
        po::store(parserResult, *vm_);
        po::notify(*vm_);
    }
    catch(po::error_with_option_name& e)
    {
        std::cout << e.what() << "\n"
                  << "\n"
                  << getHelp() << std::endl;
        return 1;
    }

    return NA;
}

int ProgramOptions::preApplicationActions()
{
    /*
     * User options
     */
    if (vm_->count(O_HELP))
    {
        std::cout << getHelp() << std::endl;
        return 0;
    }

    if (vm_->count(O_VERSION))
    {
        std::cout << DesktopUtil::Versions::kullodesktopVersion().toString() << std::endl;
        return 0;
    }

    /*
     * Developer options
     */
    if (vm_->count(O_OS))
    {
        std::cout << DesktopUtil::OsDetection::name().toStdString() << std::endl;
        return 0;
    }

    if (vm_->count(O_VERSIONS))
    {
        for (const auto pair : DesktopUtil::Versions::all())
        {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
        return 0;
    }

    if (vm_->count(O_QT_BUILD_VERSION))
    {
        std::cout << DesktopUtil::Versions::qtBuildVersion() << std::endl;
        return 0;
    }

    if (vm_->count(O_CRYPTO_INFO))
    {
        for (const auto &row : Kullo::Crypto::Info::getImplementationInfos())
        {
            std::cout << row.first << ": " << row.second << std::endl;
        }
        return 0;
    }

    if (vm_->count(O_LOG_FORMAT_EXTENDED))
    {
        logFormatExtended_ = true;
    }

    return NA;
}

int ProgramOptions::postApplicationActions(Applications::KulloApplication &app)
{
    /*
     * Developer options
     */
    if (vm_->count(O_PATHS))
    {
        std::cout << "Application dir:\n"
                  << "  " << QCoreApplication::applicationDirPath().toStdString() << "\n" << std::endl;
        std::cout << "Library paths:" << std::endl;
        for (const QString &path : app.libraryPaths())
        {
            std::cout << "  " << path.toStdString() << std::endl;
        }
        return 0;
    }

    if (vm_->count(O_FAKE_LONG_MIGRATION))
    {
        Applications::KulloApplication::FAKE_LONG_MIGRATION = true;
    }

    if (vm_->count(O_TEST))
    {
        Applications::KulloApplication::TEST_MODE = true;
        QTimer::singleShot(10*1000, &app, SLOT(quit()));
    }

    return NA;
}

void ProgramOptions::crashtestActions()
{
    /*
     * Developer options
     */
    if (vm_->count(O_CRASHTEST_NULLPTR))
    {
        Kullo::Util::Crash::nullptrDereference();
    }

    if (vm_->count(O_CRASHTEST_THROW))
    {
        Kullo::Util::Crash::throwException();
    }
}

std::string ProgramOptions::getHelp()
{
    std::stringstream out;
    out << *optionsUser_
        << "" << std::endl // Empty line
        << *optionsDeveloper_;
    std::string outStr = out.str();
    Kullo::Util::Strings::trim(outStr);
    return outStr;
}

}
}
