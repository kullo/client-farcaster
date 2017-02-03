/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "versions.h"

#include <QtGlobal>
#include <cctype>
#include <algorithm>
#include <kulloclient/api/Client.h>
#include <kulloclient/util/assert.h>
#include <httpclient/httpclientfactoryimpl.h>

namespace {

const std::string KULLO_DESKTOP_VERSION = "48.0.0";

bool charCiLess(char c1, char c2)
{
    return std::tolower(c1) < std::tolower(c2);
}

struct stringCiLess
{
    bool operator() (const std::string &lhs, const std::string &rhs) const
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                            rhs.begin(), rhs.end(),
                                            charCiLess);
    }
};

}

namespace DesktopUtil {

std::vector<std::pair<std::string, std::string>> Versions::all()
{
    auto sources = std::vector<std::unordered_map<std::string, std::string>>{
        Kullo::Api::Client::create()->versions(),
        HttpClient::HttpClientFactoryImpl().versions(),
        desktopVersions()
    };

    auto sortedTmp = std::map<std::string, std::string, stringCiLess>();

    for (const auto &source : sources)
    {
        for (const auto &pair: source)
        {
            sortedTmp[pair.first] = pair.second;
        }
    }

    auto out = std::vector<std::pair<std::string, std::string>>();

    for (const auto &keyValuePair : sortedTmp)
    {
        out.emplace_back(keyValuePair);
    }

    return out;
}

std::unordered_map<std::string, std::string> Versions::desktopVersions()
{
    return std::unordered_map<std::string, std::string>{
        { "Qt", qVersion() }
    };
}

std::string Versions::qtBuildVersion()
{
    return std::string(QT_VERSION_STR);
}

KulloVersion Versions::kullodesktopVersion()
{
    return KulloVersion(KULLO_DESKTOP_VERSION);
}

}
