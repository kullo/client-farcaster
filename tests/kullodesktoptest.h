/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <gmock/gmock.h>
#include <desktoputil/metatypes.h>
#include <desktoputil/qtypestreamers.h>

class KulloDesktopTest : public ::testing::Test
{
public:
    KulloDesktopTest()
    {
        DesktopUtil::registerMetaTypes();
    }
};
