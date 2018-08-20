/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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
