/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
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

// Those latin1 special chars work on Windows
// tested: https://gist.github.com/webmaster128/0f1466beb850ec095e38
TEST_F(Filesystem, prepareTmpFilenameLatin1Chars)
{
    // lower half of latin 1 table minus NBSP and SHY
    const auto latin1_chars = std::vector<std::string> {
            "_", "¡", "¢", "£", "¤", "¥", "¦", "§", "¨", "©", "ª", "«", "¬", "®", "_", "¯",
            "°", "±", "²", "³", "´", "µ", "¶", "·", "¸", "¹", "º", "»", "¼", "½", "¾", "¿",
            "À", "Á", "Â", "Ã", "Ä", "Å", "Æ", "Ç", "È", "É", "Ê", "Ë", "Ì", "Í", "Î", "Ï",
            "Ð", "Ñ", "Ò", "Ó", "Ô", "Õ", "Ö", "×", "Ø", "Ù", "Ú", "Û", "Ü", "Ý", "Þ", "ß",
            "à", "á", "â", "ã", "ä", "å", "æ", "ç", "è", "é", "ê", "ë", "ì", "í", "î", "ï",
            "ð", "ñ", "ò", "ó", "ô", "õ", "ö", "÷", "ø", "ù", "ú", "û", "ü", "ý", "þ", "ÿ"
            };

    for (const auto c : latin1_chars)
    {
        auto filename = QString::fromStdString("file-" + c + ".txt");
        EXPECT_THAT(DesktopUtil::Filesystem::prepareTmpFilename(filename),
                    Eq(filename));
    }
}

#ifdef Q_OS_UNIX
TEST_F(Filesystem, prepareTmpFilenameUnixUnmodifies)
{
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
        QString str(R"(file-^.txt)");
        EXPECT_EQ("file-_.txt", DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
    {
        QString str(R"(file-\.txt)");
        EXPECT_EQ("file-_.txt", DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
    {
        QString str(R"(file-/.txt)");
        EXPECT_EQ("file-_.txt", DesktopUtil::Filesystem::prepareTmpFilename(str));
    }
}
#endif

TEST_F(Filesystem, shortenSourcePath)
{
    auto inLinux = "/home/me/workspace/libkullo/kulloclient/protocol/baseclient.cpp";
    EXPECT_THAT(DesktopUtil::Filesystem::shortenSourcePath(inLinux),
                StrEq("baseclient.cpp"));

    auto inWindows = R"(C:\Program Files (x86)\My Ci System\projects\libkullo\libkullo\kulloclient\protocol\baseclient.cpp)";
    EXPECT_THAT(DesktopUtil::Filesystem::shortenSourcePath(inWindows),
                StrEq("baseclient.cpp"));

    auto inRelative = R"(..\..\farcaster\kullodesktop\util\logfilecleaner.cpp)";
    EXPECT_THAT(DesktopUtil::Filesystem::shortenSourcePath(inRelative),
                StrEq("logfilecleaner.cpp"));
}
