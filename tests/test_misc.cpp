/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "kullodesktoptest.h"

class Misc : public KulloDesktopTest
{
};

TEST_F(Misc, CppAndQtReleaseConstantsAreEqual)
{
    bool valuesEqual = false;

#ifdef NDEBUG
#ifdef QT_NO_DEBUG
    valuesEqual = true;
#endif
#endif

#ifndef NDEBUG
#ifndef QT_NO_DEBUG
    valuesEqual = true;
#endif
#endif

    EXPECT_TRUE(valuesEqual);
}
