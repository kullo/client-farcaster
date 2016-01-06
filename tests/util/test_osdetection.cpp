/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "../kullodesktoptest.h"

#include <string>
#include <desktoputil/osdetection.h>

using namespace testing;

class OsDetection : public KulloDesktopTest
{
};

TEST_F(OsDetection, osDefinitions)
{
    int osCounter = 0;

#ifdef K_OS_LINUX32
    ++osCounter;
    EXPECT_THAT(std::string(K_OS_NAME), Eq("linux32"));
#endif
#ifdef K_OS_LINUX64
    ++osCounter;
    EXPECT_THAT(std::string(K_OS_NAME), Eq("linux64"));
#endif
#ifdef K_OS_WINDOWS
    ++osCounter;
    EXPECT_THAT(std::string(K_OS_NAME), Eq("windows"));
#endif
#ifdef K_OS_OSX
    ++osCounter;
    EXPECT_THAT(std::string(K_OS_NAME), Eq("osx"));
#endif

    EXPECT_THAT(std::string(K_OS_NAME), Ne(""));
    EXPECT_THAT(osCounter, Eq(1));
}

TEST_F(OsDetection, debug)
{
#ifdef QT_NO_DEBUG
    // release
    EXPECT_THAT(DesktopUtil::OsDetection::debug(), Eq(false));
#else
    // debug
    EXPECT_THAT(DesktopUtil::OsDetection::debug(), Eq(true));
#endif
}
