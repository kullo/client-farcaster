/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <map>
#include <string>
#include <vector>
#include <unordered_map>

#include "desktoputil/kulloversion.h"

namespace DesktopUtil {

class Versions
{
public:
    static std::vector<std::pair<std::string, std::string>> all();
    static std::unordered_map<std::string, std::string> desktopVersions();
    static std::string qtBuildVersion();
    static KulloVersion kullodesktopVersion();
};

}
