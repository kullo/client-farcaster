/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
