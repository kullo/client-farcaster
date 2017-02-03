/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "../kullodesktoptest.h"

#include <QPoint>
#include <QString>
#include <QSize>
#include <QStringList>
#include <QUrl>
#include <QVariant>

#include <ostream>

#include <kulloclient/util/librarylogger.h>

using namespace testing;

class QTypeStreamers : public KulloDesktopTest
{
};

// Missing typs: QFileInfo, QFileInfoList

TEST_F(QTypeStreamers, logQChar)
{
    QChar empty;
    QChar simple(QLatin1Char('a'));

    // Don't use QStringLiteral with special chars
    // https://bugreports.qt.io/browse/QTBUG-46375
    QChar special(QString("ö").at(0));

    Log.i() << empty;
    Log.i() << simple;
    Log.i() << special;

    {
        std::ostringstream out;
        out << empty;
        EXPECT_EQ("QChar::Null", out.str());
    }
    {
        std::ostringstream out;
        out << simple;
        EXPECT_EQ("'a'", out.str());
    }
    {
        std::ostringstream out;
        out << special;
        EXPECT_EQ("'ö'", out.str());
    }
}

TEST_F(QTypeStreamers, logQString)
{
    QString empty;
    QString content("foo");
    QString special("fooÄ");

    Log.i() << empty;
    Log.i() << content;
    Log.i() << special;

    {
        std::ostringstream out;
        out << empty;
        EXPECT_EQ("\"\"", out.str());
    }
    {
        std::ostringstream out;
        out << content;
        EXPECT_EQ("\"foo\"", out.str());
    }
    {
        std::ostringstream out;
        out << special;
        EXPECT_EQ("\"fooÄ\"", out.str());
    }
}

TEST_F(QTypeStreamers, logQStringList)
{
    QStringList listEmpty;
    QStringList listOne = (QStringList() << QStringLiteral("foo"));
    QStringList listMulti = (QStringList() << QStringLiteral("foo") << QStringLiteral("bar"));

    Log.i() << listEmpty;
    Log.i() << listOne;
    Log.i() << listMulti;

    {
        std::ostringstream out;
        out << listEmpty;
        EXPECT_EQ("[]", out.str());
    }
    {
        std::ostringstream out;
        out << listOne;
        EXPECT_EQ("[\"foo\"]", out.str());
    }
    {
        std::ostringstream out;
        out << listMulti;
        EXPECT_EQ("[\"foo\", \"bar\"]", out.str());
    }
}

TEST_F(QTypeStreamers, logQSize)
{
    Log.i() << QSize();
    Log.i() << QSize(1024, 768);

    {
        std::ostringstream out;
        out << QSize();
        EXPECT_EQ("-1x-1", out.str());
    }
    {
        std::ostringstream out;
        out << QSize(1024, 768);
        EXPECT_EQ("1024x768", out.str());
    }
}

TEST_F(QTypeStreamers, logQPoint)
{
    Log.i() << QPoint();
    Log.i() << QPoint(4, 3);
    Log.i() << QPoint(-1, 3);

    {
        std::ostringstream out;
        out << QPoint();
        EXPECT_EQ("(0,0)", out.str());
    }
    {
        std::ostringstream out;
        out << QPoint(4, 3);
        EXPECT_EQ("(4,3)", out.str());
    }
    {
        std::ostringstream out;
        out << QPoint(-1, 3);
        EXPECT_EQ("(-1,3)", out.str());
    }
}

TEST_F(QTypeStreamers, logQUrl)
{
    QUrl empty;
    QUrl contentHttp         = QUrl("http://example.com/");
    QUrl contentHttps        = QUrl("https://example.com/");
    QUrl contentSpace        = QUrl("http://example.com/some path/");
    QUrl contentSpaceEncoded = QUrl("http://example.com/some%20path/");
    QUrl contentKulloAddress = QUrl("http://example.com/hi%23kullo.net/messages");
    QUrl contentQuery        = QUrl("http://example.com/?var=1");
    QUrl contentFragment     = QUrl("http://example.com/#headline5");

    Log.i() << empty;
    Log.i() << contentHttp;
    Log.i() << contentHttps;
    Log.i() << contentSpace;
    Log.i() << contentSpaceEncoded;
    Log.i() << contentKulloAddress;
    Log.i() << contentQuery;
    Log.i() << contentFragment;

    {
        std::ostringstream out;
        out << empty;
        EXPECT_EQ("‹›", out.str());
    }
    {
        std::ostringstream out;
        out << contentHttp;
        EXPECT_EQ("‹http://example.com/›", out.str());
    }
    {
        std::ostringstream out;
        out << contentHttps;
        EXPECT_EQ("‹https://example.com/›", out.str());
    }
    {
        std::ostringstream out;
        out << contentSpace;
        EXPECT_EQ("‹http://example.com/some path/›", out.str());
    }
    {
        std::ostringstream out;
        out << contentSpaceEncoded;
        EXPECT_EQ("‹http://example.com/some path/›", out.str());
    }
    {
        std::ostringstream out;
        out << contentKulloAddress;
        EXPECT_EQ("‹http://example.com/hi%23kullo.net/messages›", out.str());
    }
    {
        std::ostringstream out;
        out << contentQuery;
        EXPECT_EQ("‹http://example.com/?var=1›", out.str());
    }
    {
        std::ostringstream out;
        out << contentFragment;
        EXPECT_EQ("‹http://example.com/#headline5›", out.str());
    }
}

TEST_F(QTypeStreamers, logQVariant)
{
    Log.i() << QVariant();
    Log.i() << QVariant(true) << ", " << QVariant(false);
    Log.i() << QVariant(123) << ", " << QVariant(-123);
    Log.i() << QVariant("abc") << ", " << QVariant("ABC");
}
