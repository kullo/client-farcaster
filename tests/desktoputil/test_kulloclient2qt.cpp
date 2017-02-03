/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "../kullodesktoptest.h"

#include <QDateTime>
#include <desktoputil/kulloclient2qt.h>

using namespace testing;

class KulloClient2Qt : public KulloDesktopTest
{
};

TEST_F(KulloClient2Qt, toQDateTime)
{
    QDateTime qdt1(QDate(2024, 1, 1), QTime(0, 0), Qt::UTC);
    QDateTime qdt2(QDate(2024, 1, 1), QTime(18, 45, 12), Qt::UTC);
    QDateTime qdt3(QDate(2024, 1, 1), QTime(12, 4, 8, 0), Qt::UTC); // ms is zero

    EXPECT_THAT(DesktopUtil::KulloClient2Qt::toQDateTime(Kullo::Api::DateTime(2024, 1, 1,  0,  0,  0, 0)), Eq(qdt1));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::toQDateTime(Kullo::Api::DateTime(2024, 1, 1, 18, 45, 12, 0)), Eq(qdt2));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::toQDateTime(Kullo::Api::DateTime(2024, 1, 1, 12,  4,  8, 0)), Eq(qdt3));
}

TEST_F(KulloClient2Qt, fromUtf8)
{
    QByteArray a;
    QByteArray b = QByteArrayLiteral("ab");
    QByteArray c = QByteArrayLiteral("ö");
    QByteArray d = QByteArrayLiteral("äöüÄÖÜß");
    QByteArray e = QByteArrayLiteral("⚡Ƿ➜");
    QByteArray f = QByteArrayLiteral("\xc3\x9f"); // echo -n "ß" | hexdump -C

    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(a), Eq(std::string("")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(b), Eq(std::string("ab")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(c), Eq(std::string("ö")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(d), Eq(std::string("äöüÄÖÜß")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(e), Eq(std::string("⚡Ƿ➜")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(f), Eq(std::string("ß")));
}
