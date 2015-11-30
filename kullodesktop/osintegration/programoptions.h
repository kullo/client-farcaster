/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <string>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace OsIntegration {

class ProgramOptions
{
public:
    static int NA;

    ProgramOptions();
    ~ProgramOptions();

    int parse(int argc, char *argv[]);
    int preApplicationActions();
    int postApplicationActions(Applications::KulloApplication &app);
    void crashtestActions();

    bool logFormatExtended_ = false;

private:
    std::string getHelp();
    std::unique_ptr<boost::program_options::variables_map> vm_;
    std::unique_ptr<boost::program_options::options_description> optionsAll_;
    std::unique_ptr<boost::program_options::options_description> optionsUser_;
    std::unique_ptr<boost::program_options::options_description> optionsDeveloper_;
};

}
}
