/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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

TEST_F(Filesystem, increaseFilenameCounter)
{
    {
        auto in = QStringLiteral("file:///home/me/project/baseclient.cpp");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/baseclient (1).cpp"));
    }

    {
        auto in = QStringLiteral("file:///home/me/project/baseclient (1).cpp");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/baseclient (2).cpp"));
    }

    {
        auto in = QStringLiteral("file:///home/me/project/baseclient (9).cpp");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/baseclient (10).cpp"));
    }

    {
        auto in = QStringLiteral("file:///home/me/project/baseclient (9999).cpp");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/baseclient (10000).cpp"));
    }

    {
        // double file extension: 0 -> 1
        auto in = QStringLiteral("file:///home/me/project/archive.tar.bz2");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/archive (1).tar.bz2"));
    }

    {
        // double file extension: 1 -> 2
        auto in = QStringLiteral("file:///home/me/project/archive (1).tar.bz2");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/archive (2).tar.bz2"));
    }

    {
        // extension only (not beautiful but consistent): 0 -> 1
        auto in = QStringLiteral("file:///home/me/project/.htaccess");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/ (1).htaccess"));
    }

    {
        // extension only (not beautiful but consistent): 1 -> 2
        auto in = QStringLiteral("file:///home/me/project/ (1).htaccess");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/ (2).htaccess"));
    }

    {
        // number not generated by me (leading zero)
        auto in = QStringLiteral("file:///home/me/project/baseclient (01).cpp");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/baseclient (01) (1).cpp"));
    }

    {
        // number not generated by me (not a number)
        auto in = QStringLiteral("file:///home/me/project/baseclient ().cpp");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/baseclient () (1).cpp"));
    }

    {
        // number not generated by me (negative)
        auto in = QStringLiteral("file:///home/me/project/baseclient (-1).cpp");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/baseclient (-1) (1).cpp"));
    }

    {
        // number not generated by me (space missing)
        auto in = QStringLiteral("file:///home/me/project/baseclient(8).cpp");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrEq("file:///home/me/project/baseclient(8) (1).cpp"));
    }
}

TEST_F(Filesystem, increaseFilenameCounterWindows)
{
    {
        auto in = QStringLiteral("file:///C:/Program Files (x86)/7-Zip/7-zip.dll");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrCaseEq("file:///C:/Program Files (x86)/7-Zip/7-zip (1).dll"));
    }

    {
        auto in = QStringLiteral("file:///C:/Program Files (x86)/7-Zip/7-zip (1).dll");
        auto actual = DesktopUtil::Filesystem::increaseFilenameCounter(in).toString().toStdString();
        EXPECT_THAT(actual, StrCaseEq("file:///C:/Program Files (x86)/7-Zip/7-zip (2).dll"));
    }
}
