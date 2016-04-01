/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <cstdint>
#include <string>

// http://stackoverflow.com/questions/22240973/major-and-minor-macros-defined-in-sys-sysmacros-h-pulled-in-by-iterator
#undef major
#undef minor

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
    int major() const;
    int minor() const;
    int patch() const;

    friend std::ostream &operator<<(std::ostream &out, const KulloVersion &version);

private:
    std::uint32_t major_;
    std::uint32_t minor_;
    std::uint32_t patch_;
    bool preview_ = false;
};

}
