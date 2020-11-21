/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "../kullodesktoptest.h"

#include <desktoputil/paths.h>
#include <kulloclient/util/assert.h>
#include <QCoreApplication>

using namespace testing;

namespace {
int DUMMY_ARGC = 1;
char DUMMY_ARGV_0[9] = "mybinary";
char* DUMMY_ARGV[] = { DUMMY_ARGV_0 };
}

class Paths : public KulloDesktopTest
{
};

TEST_F(Paths, failsWhenApplicationNotAvailable)
{
    EXPECT_THROW(DesktopUtil::kulloPaths(), Kullo::Util::AssertionFailed);

    {
        // no owner/name
        QCoreApplication app(DUMMY_ARGC, DUMMY_ARGV);
        EXPECT_THROW(DesktopUtil::kulloPaths(), Kullo::Util::AssertionFailed);
    }

    {
        // no living application instance
        QCoreApplication::setOrganizationName("Developer Inc.");
        QCoreApplication::setApplicationName("MyApp");
        EXPECT_THROW(DesktopUtil::kulloPaths(), Kullo::Util::AssertionFailed);
    }
}

TEST_F(Paths, worksWhenApplicationAvailable)
{
    QCoreApplication app(DUMMY_ARGC, DUMMY_ARGV);
    app.setOrganizationName("Developer Inc.");
    app.setApplicationName("MyApp");

    auto paths = DesktopUtil::kulloPaths();

    EXPECT_THAT(paths.BINARY_DIR, Ne(QString()));
    EXPECT_THAT(paths.CRASHREPORTER_ABSFILEPATH, Ne(QString()));
    EXPECT_THAT(paths.DATABASE_DIR, Ne(QString()));
    EXPECT_THAT(paths.KULLO_DIR, Ne(QString()));
    EXPECT_THAT(paths.UPDATES_DIR, Ne(QString()));
}

TEST_F(Paths, logfileDirDoesSomething)
{
    auto logfiledir = DesktopUtil::logfileDir();
    EXPECT_THAT(logfiledir, Ne(QString()));
}

TEST_F(Paths, defaultSaveAttachmentsDirDoesSomething)
{
    auto defaultSaveAttachmentsDir = DesktopUtil::defaultSaveAttachmentsDir();
    EXPECT_THAT(defaultSaveAttachmentsDir, Ne(QString()));
}
