/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "desktoputil/kulloversion.h"

#include <algorithm>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace DesktopUtil {

namespace {
const std::regex SIMPLE_VERSION_REGEX("[0-9]+\\.[0-9]+\\.[0-9]+");
}

KulloVersion::KulloVersion(const std::string &inputStr)
{
    std::string version;
    std::string suffix;

    auto sepPos = std::find(inputStr.cbegin(), inputStr.cend(), '-');

    if (sepPos != inputStr.cend())
    {
        std::copy(inputStr.cbegin(), sepPos, std::back_inserter(version));
        std::copy(sepPos + 1, inputStr.cend(), std::back_inserter(suffix));
        if (suffix == "prev")
        {
            preview_ = true;
        }
        else
        {
            throw std::invalid_argument("Invalid suffix '" + suffix + "'");
        }
    }
    else
    {
        version = inputStr;
    }

    if (!std::regex_match(version, SIMPLE_VERSION_REGEX))
    {
        throw std::invalid_argument("Version string has unsupported format. Must be: X.Y.Z");
    }

    std::istringstream versionStream(version);
    std::string vmajor;
    std::string vminor;
    std::string vpatch;
    getline(versionStream, vmajor, '.');
    getline(versionStream, vminor, '.');
    getline(versionStream, vpatch, '.');

    if (vmajor.empty()) throw std::invalid_argument("1st version part is empty");
    if (vminor.empty()) throw std::invalid_argument("2nd version part is empty");
    if (vpatch.empty()) throw std::invalid_argument("3rd version part is empty");

    std::istringstream vmajorStream(vmajor);
    vmajorStream >> major_;
    if (vmajorStream.fail()) throw std::invalid_argument("1st version part could not be converted to integer.");

    std::istringstream vminorStream(vminor);
    vminorStream >> minor_;
    if (vminorStream.fail()) throw std::invalid_argument("2nd version part could not be converted to integer.");

    std::istringstream vpatchStream(vpatch);
    vpatchStream >> patch_;
    if (vpatchStream.fail()) throw std::invalid_argument("3rd version part could not be converted to integer.");
}

std::string KulloVersion::toString() const
{
    return std::to_string(major_) + "." +
           std::to_string(minor_) + "." +
           std::to_string(patch_) +
           (preview_ ? "-prev" : "");
}

bool KulloVersion::operator==(const KulloVersion &other) const
{
    return major_ == other.major_
        && minor_ == other.minor_
        && patch_ == other.patch_
        && preview_ == other.preview_;
}

bool KulloVersion::operator!=(const KulloVersion &other) const
{
    return !(*this == other);
}

bool KulloVersion::operator<(const KulloVersion &other) const
{
    if (major_ < other.major_) return true;
    if (major_ > other.major_) return false;
    if (minor_ < other.minor_) return true;
    if (minor_ > other.minor_) return false;
    if (patch_ < other.patch_) return true;
    if (patch_ > other.patch_) return false;
    if ( preview_ && !other.preview_) return true;
    if (!preview_ &&  other.preview_) return false;
    return false; // versions equal
}

bool KulloVersion::operator>(const KulloVersion &other) const
{
    return *this != other && !(*this < other);
}

bool KulloVersion::isMajorUpgradeOf(const KulloVersion &other) const
{
    return this->major_ > other.major_;
}

bool KulloVersion::isMinorUpgradeOf(const KulloVersion &other) const
{
    if (isMajorUpgradeOf(other)) return true;
    return this->minor_ > other.minor_;
}

bool KulloVersion::isPatchUpgradeOf(const KulloVersion &other) const
{
    if (isMajorUpgradeOf(other)) return true;
    if (isMinorUpgradeOf(other)) return true;
    return this->patch_ > other.patch_;
}

int KulloVersion::majorVersion() const
{
    return major_;
}

int KulloVersion::minorVersion() const
{
    return minor_;
}

int KulloVersion::patchVersion() const
{
    return patch_;
}

std::ostream &operator<<(std::ostream &out, const KulloVersion &version)
{
    out << version.toString();
    return out;
}

}
