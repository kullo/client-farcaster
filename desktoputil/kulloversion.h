/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <cstdint>
#include <string>

namespace DesktopUtil {

class KulloVersion
{
public:
    KulloVersion(const std::string &inputStr);
    std::string toString() const;
    bool operator==(const KulloVersion &other) const;
    bool operator!=(const KulloVersion &other) const;
    bool operator<(const KulloVersion &other) const;
    bool operator>(const KulloVersion &other) const;
    bool isMajorUpgradeOf(const KulloVersion &other) const;
    bool isMinorUpgradeOf(const KulloVersion &other) const;
    bool isPatchUpgradeOf(const KulloVersion &other) const;

    // Do not use simple names major() and minor() because those are macros
    // and cause trouble with -std=gnu++11 as well as clang.
    // See http://stackoverflow.com/questions/22240973/major-and-minor-macros-defined-in-sys-sysmacros-h-pulled-in-by-iterator
    int majorVersion() const;
    int minorVersion() const;
    int patchVersion() const;

    friend std::ostream &operator<<(std::ostream &out, const KulloVersion &version);

private:
    std::uint32_t major_ = 0;
    std::uint32_t minor_ = 0;
    std::uint32_t patch_ = 0;
    bool preview_ = false;
};

}
