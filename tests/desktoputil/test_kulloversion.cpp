/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "../kullodesktoptest.h"

#include <exception>

#include <desktoputil/kulloversion.h>


using namespace testing;

class KulloVersion : public KulloDesktopTest
{
};

TEST_F(KulloVersion, format)
{
    EXPECT_NO_THROW(DesktopUtil::KulloVersion("1.2.3"));
    EXPECT_NO_THROW(DesktopUtil::KulloVersion("0.1.2"));
    EXPECT_NO_THROW(DesktopUtil::KulloVersion("0.0.1"));
    EXPECT_NO_THROW(DesktopUtil::KulloVersion("1.2.0003"));

    // number of positions
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("1.2"));
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("1.2.3.4"));

    // trailing dot
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("1.2."));
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("1.2.3."));

    // negative
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("-1.2.0003"));
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("1.-2.0003"));
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("1.2.-0003"));

    // letter
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("1.2.a"));
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("1.a.3"));
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("a.2.3"));

    // additional stuff
    EXPECT_ANY_THROW(DesktopUtil::KulloVersion("0.20.4 (Kopie)"));
}

TEST_F(KulloVersion, equals)
{
    EXPECT_EQ(DesktopUtil::KulloVersion("0.12.4"), DesktopUtil::KulloVersion("0.12.4"));

    EXPECT_EQ(DesktopUtil::KulloVersion("1.2.3"), DesktopUtil::KulloVersion("01.2.3"));
    EXPECT_EQ(DesktopUtil::KulloVersion("1.2.3"), DesktopUtil::KulloVersion("1.02.3"));
    EXPECT_EQ(DesktopUtil::KulloVersion("1.2.3"), DesktopUtil::KulloVersion("1.2.03"));

    EXPECT_EQ(DesktopUtil::KulloVersion("0001.2.3"), DesktopUtil::KulloVersion("1.2.3"));
    EXPECT_EQ(DesktopUtil::KulloVersion("1.0002.3"), DesktopUtil::KulloVersion("1.2.3"));
    EXPECT_EQ(DesktopUtil::KulloVersion("1.2.0003"), DesktopUtil::KulloVersion("1.2.3"));
}

TEST_F(KulloVersion, greater)
{
    EXPECT_GT(DesktopUtil::KulloVersion("0.12.4"), DesktopUtil::KulloVersion("0.12.3"));
    EXPECT_GT(DesktopUtil::KulloVersion("0.13.4"), DesktopUtil::KulloVersion("0.12.4"));
    EXPECT_GT(DesktopUtil::KulloVersion("0.13.4"), DesktopUtil::KulloVersion("0.12.5"));

    EXPECT_GT(DesktopUtil::KulloVersion("1.0.0"), DesktopUtil::KulloVersion("0.12.300"));
    EXPECT_GT(DesktopUtil::KulloVersion("1.0.0"), DesktopUtil::KulloVersion("0.12.487"));
    EXPECT_GT(DesktopUtil::KulloVersion("1.0.0"), DesktopUtil::KulloVersion("0.992.52"));
}

TEST_F(KulloVersion, less)
{
    EXPECT_LT(DesktopUtil::KulloVersion("0.12.3"), DesktopUtil::KulloVersion("0.12.4"));
    EXPECT_LT(DesktopUtil::KulloVersion("0.12.4"), DesktopUtil::KulloVersion("0.13.4"));
    EXPECT_LT(DesktopUtil::KulloVersion("0.12.5"), DesktopUtil::KulloVersion("0.14.4"));

    EXPECT_LT(DesktopUtil::KulloVersion("0.12.300"), DesktopUtil::KulloVersion("1.0.0"));
    EXPECT_LT(DesktopUtil::KulloVersion("0.12.487"), DesktopUtil::KulloVersion("1.0.0"));
    EXPECT_LT(DesktopUtil::KulloVersion("0.992.52"), DesktopUtil::KulloVersion("1.0.0"));
}

TEST_F(KulloVersion, preview)
{
    EXPECT_NO_THROW(DesktopUtil::KulloVersion("1.0.0-prev"));

    EXPECT_NE(DesktopUtil::KulloVersion("1.0.0-prev"), DesktopUtil::KulloVersion("1.0.0"));
    EXPECT_LT(DesktopUtil::KulloVersion("1.0.0-prev"), DesktopUtil::KulloVersion("1.0.0"));
    EXPECT_LT(DesktopUtil::KulloVersion("0.992.52"), DesktopUtil::KulloVersion("1.0.0-prev"));

    EXPECT_THROW(DesktopUtil::KulloVersion("1.0.0-othersuffix"), std::invalid_argument);
    EXPECT_THROW(DesktopUtil::KulloVersion("1.0.0-"), std::invalid_argument);
}


TEST_F(KulloVersion, isMayorUpgradeOf)
{
    EXPECT_TRUE(DesktopUtil::KulloVersion("1.12.3").isMajorUpgradeOf(DesktopUtil::KulloVersion("0.12.3"))); // minor, patch equal
    EXPECT_TRUE(DesktopUtil::KulloVersion("1.12.3").isMajorUpgradeOf(DesktopUtil::KulloVersion("0.13.4"))); // minor, patch lower
    EXPECT_TRUE(DesktopUtil::KulloVersion("1.12.3").isMajorUpgradeOf(DesktopUtil::KulloVersion("0.11.2"))); // minor, patch bigger

    EXPECT_FALSE(DesktopUtil::KulloVersion("1.12.3").isMajorUpgradeOf(DesktopUtil::KulloVersion("1.12.3"))); // minor, patch equal
    EXPECT_FALSE(DesktopUtil::KulloVersion("0.12.3").isMajorUpgradeOf(DesktopUtil::KulloVersion("0.13.4"))); // minor, patch lower
    EXPECT_FALSE(DesktopUtil::KulloVersion("0.12.3").isMajorUpgradeOf(DesktopUtil::KulloVersion("0.11.2"))); // minor, patch bigger
}

TEST_F(KulloVersion, isMinorUpgradeOf)
{
    EXPECT_TRUE(DesktopUtil::KulloVersion("1.12.3").isMinorUpgradeOf(DesktopUtil::KulloVersion("0.12.3"))); // mayor upgrade is minor upgrade

    EXPECT_TRUE(DesktopUtil::KulloVersion("0.12.3").isMinorUpgradeOf(DesktopUtil::KulloVersion("0.11.3"))); // patch equal
    EXPECT_TRUE(DesktopUtil::KulloVersion("0.12.3").isMinorUpgradeOf(DesktopUtil::KulloVersion("0.11.4"))); // patch lower
    EXPECT_TRUE(DesktopUtil::KulloVersion("0.12.3").isMinorUpgradeOf(DesktopUtil::KulloVersion("0.11.2"))); // patch bigger

    EXPECT_FALSE(DesktopUtil::KulloVersion("0.12.3").isMajorUpgradeOf(DesktopUtil::KulloVersion("0.12.3"))); // patch equal
    EXPECT_FALSE(DesktopUtil::KulloVersion("0.12.3").isMajorUpgradeOf(DesktopUtil::KulloVersion("0.12.4"))); // patch lower
    EXPECT_FALSE(DesktopUtil::KulloVersion("0.12.3").isMajorUpgradeOf(DesktopUtil::KulloVersion("0.12.2"))); // patch bigger
}

TEST_F(KulloVersion, isPatchUpgradeOf)
{
    EXPECT_TRUE(DesktopUtil::KulloVersion("1.12.3").isPatchUpgradeOf(DesktopUtil::KulloVersion("0.12.3"))); // mayor upgrade is patch upgrade
    EXPECT_TRUE(DesktopUtil::KulloVersion("0.13.3").isPatchUpgradeOf(DesktopUtil::KulloVersion("0.12.3"))); // minor upgrade is patch upgrade

    EXPECT_TRUE(DesktopUtil::KulloVersion("0.12.3").isPatchUpgradeOf(DesktopUtil::KulloVersion("0.12.2"))); // patch bigger
    EXPECT_FALSE(DesktopUtil::KulloVersion("0.12.3").isPatchUpgradeOf(DesktopUtil::KulloVersion("0.12.4"))); // patch lower
}

TEST_F(KulloVersion, getters)
{
    auto version = DesktopUtil::KulloVersion("1.12.3");
    EXPECT_THAT(version.majorVersion(), Eq(1));
    EXPECT_THAT(version.minorVersion(), Eq(12));
    EXPECT_THAT(version.patchVersion(), Eq(3));
}
