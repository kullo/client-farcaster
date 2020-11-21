/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
