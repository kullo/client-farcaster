/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <string>

struct Metadata
{
    std::string uploadUrl;
    std::string productName;
    std::string productVersion;
    std::string clientGuid;
    std::string dumpPath;
    std::string processUptime;
    std::string cumulativeUptime;
    std::string email;
    std::string comments;

    bool valid();
};
