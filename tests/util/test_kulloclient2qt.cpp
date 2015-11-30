/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "../kullodesktoptest.h"

#include <QDateTime>
#include <desktoputil/kulloclient2qt.h>
#include <kulloclient/util/datetime.h>

using namespace testing;

class KulloClient2Qt : public KulloDesktopTest
{
};

TEST_F(KulloClient2Qt, toQDateTime)
{
    QDateTime qdt1(QDate(2024, 1, 1), QTime(0, 0), Qt::UTC);
    QDateTime qdt2(QDate(2024, 1, 1), QTime(18, 45, 12), Qt::UTC);
    QDateTime qdt3(QDate(2024, 1, 1), QTime(12, 4, 8, 0), Qt::UTC); // ms is null

    EXPECT_THAT(DesktopUtil::KulloClient2Qt::toQDateTime(Kullo::Util::DateTime("2024-01-01T00:00:00Z")), Eq(qdt1));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::toQDateTime(Kullo::Util::DateTime("2024-01-01T18:45:12Z")), Eq(qdt2));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::toQDateTime(Kullo::Util::DateTime("2024-01-01T12:04:08Z")), Eq(qdt3));
}

TEST_F(KulloClient2Qt, toDateTime)
{
    QDateTime qdt1(QDate(2024, 1, 1), QTime(0, 0), Qt::UTC);
    QDateTime qdt2(QDate(2024, 1, 1), QTime(18, 45, 12), Qt::UTC);
    QDateTime qdt3(QDate(2024, 1, 1), QTime(12, 4, 8, 0), Qt::UTC); // ms is null

    EXPECT_THAT(DesktopUtil::KulloClient2Qt::toDateTime(qdt1), Eq(Kullo::Util::DateTime("2024-01-01T00:00:00Z")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::toDateTime(qdt2), Eq(Kullo::Util::DateTime("2024-01-01T18:45:12Z")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::toDateTime(qdt3), Eq(Kullo::Util::DateTime("2024-01-01T12:04:08Z")));
}

TEST_F(KulloClient2Qt, fromUtf8)
{
    QByteArray a;
    QByteArray b = QByteArrayLiteral("ab");
    QByteArray c = QByteArrayLiteral("ö");
    QByteArray d = QByteArrayLiteral("äöüÄÖÜß");
    QByteArray e = QByteArrayLiteral("⚡Ƿ➜");

    // echo -n "ß" | hexdump -C
    // c3 9f = 195 159
    QByteArray f;
    f.append(195);
    f.append(159);

    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(a), Eq(std::string("")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(b), Eq(std::string("ab")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(c), Eq(std::string("ö")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(d), Eq(std::string("äöüÄÖÜß")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(e), Eq(std::string("⚡Ƿ➜")));
    EXPECT_THAT(DesktopUtil::KulloClient2Qt::fromUtf8(f), Eq(std::string("ß")));
}
