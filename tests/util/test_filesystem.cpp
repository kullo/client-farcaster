/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "../kullodesktoptest.h"

#include <desktoputil/filesystem.h>

using namespace testing;

class Filesystem : public KulloDesktopTest
{
};

TEST_F(Filesystem, prepareTmpFilenameBasic)
{
    QString simple("simple.txt");
    EXPECT_EQ(simple, DesktopUtil::Filesystem::prepareTmpFilename(simple));

    QString number("simple0123456789.txt");
    EXPECT_EQ(number, DesktopUtil::Filesystem::prepareTmpFilename(number));

    QString space("simple space.txt");
    EXPECT_EQ(space, DesktopUtil::Filesystem::prepareTmpFilename(space));

    QString dot("d.ot.txt");
    EXPECT_EQ(dot, DesktopUtil::Filesystem::prepareTmpFilename(dot));

    QString minus("mi-nus.txt");
    EXPECT_EQ(minus, DesktopUtil::Filesystem::prepareTmpFilename(minus));

    QString underscore("under_score.txt");
    EXPECT_EQ(underscore, DesktopUtil::Filesystem::prepareTmpFilename(underscore));
}

#ifdef Q_OS_UNIX
TEST_F(Filesystem, prepareTmpFilenameUnixUnmodifies)
{
    {
        QString str(R"(file-ö.txt)");
        EXPECT_EQ(str, DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
    {
        QString str(R"(file-ß.txt)");
        EXPECT_EQ(str, DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
    {
        QString str(R"(file-^.txt)");
        EXPECT_EQ(str, DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
    {
        QString str(R"(file-\.txt)");
        EXPECT_EQ(str, DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
}

TEST_F(Filesystem, prepareTmpFilenameUnixModified)
{
    {
        QString str(R"(file-/.txt)");
        EXPECT_EQ("file-_.txt", DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
}
#endif

#ifdef Q_OS_WIN
TEST_F(Filesystem, prepareTmpFilenameWindowsModified)
{
    {
        QString str(R"(file-ö.txt)");
        EXPECT_EQ("file-_.txt", DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
    {
        QString str(R"(file-ß.txt)");
        EXPECT_EQ("file-_.txt", DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
    {
        QString str(R"(file-^.txt)");
        EXPECT_EQ("file-_.txt", DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
    {
        QString str(R"(file-\.txt)");
        EXPECT_EQ("file-_.txt", DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
}
#endif
