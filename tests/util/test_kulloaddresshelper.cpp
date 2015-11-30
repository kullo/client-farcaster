/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "../kullodesktoptest.h"

#include <QStringList>
#include <string>
#include <desktoputil/kulloaddresshelper.h>

using namespace testing;

class KulloAddressHelper : public KulloDesktopTest
{
};

TEST_F(KulloAddressHelper, ok)
{
    {
        QString addr("hi#kullo.net");
        EXPECT_TRUE(DesktopUtil::KulloAddressHelper::isValidKulloAddress(addr));
    }
    {
        QString addr("");
        EXPECT_FALSE(DesktopUtil::KulloAddressHelper::isValidKulloAddress(addr));
    }
    {
        QString addr("foo");
        EXPECT_FALSE(DesktopUtil::KulloAddressHelper::isValidKulloAddress(addr));
    }
    {
        QString addr("hi@kullo.net");
        EXPECT_FALSE(DesktopUtil::KulloAddressHelper::isValidKulloAddress(addr));
    }
    {
        QString addr("hi#kullo.net ");
        EXPECT_FALSE(DesktopUtil::KulloAddressHelper::isValidKulloAddress(addr));
    }
    {
        QString addr(" hi#kullo.net");
        EXPECT_FALSE(DesktopUtil::KulloAddressHelper::isValidKulloAddress(addr));
    }
}

TEST_F(KulloAddressHelper, equals)
{
    {
        QString addr1("hi#kullo.net");
        QString addr2("hi#Kullo.net");
        EXPECT_TRUE(DesktopUtil::KulloAddressHelper::kulloAddressEqual(addr1, addr2));
    }
    {
        QString addr1("hi#kullo.net");
        QString addr2("Hi#kullo.net");
        EXPECT_TRUE(DesktopUtil::KulloAddressHelper::kulloAddressEqual(addr1, addr2));
    }
    {
        QString addr1("hi1#kullo.net");
        QString addr2("hi2#kullo.net");
        EXPECT_FALSE(DesktopUtil::KulloAddressHelper::kulloAddressEqual(addr1, addr2));
    }
    {
        QString addr1("hi#kullo.net");
        QString addr2("hi#kullo.com");
        EXPECT_FALSE(DesktopUtil::KulloAddressHelper::kulloAddressEqual(addr1, addr2));
    }
    {
        QString addr1("hi#kullo.net");
        QString addr2("hi#example.net");
        EXPECT_FALSE(DesktopUtil::KulloAddressHelper::kulloAddressEqual(addr1, addr2));
    }
}


TEST_F(KulloAddressHelper, isValidKulloDomain)
{
    EXPECT_THAT(DesktopUtil::KulloAddressHelper::isValidKulloDomain("kullo.net"), Eq(true));
    EXPECT_THAT(DesktopUtil::KulloAddressHelper::isValidKulloDomain("kullo.dev"), Eq(true));
    EXPECT_THAT(DesktopUtil::KulloAddressHelper::isValidKulloDomain("kullo.local"), Eq(true));
    EXPECT_THAT(DesktopUtil::KulloAddressHelper::isValidKulloDomain(""), Eq(false));
    EXPECT_THAT(DesktopUtil::KulloAddressHelper::isValidKulloDomain("kullo"), Eq(false));
    EXPECT_THAT(DesktopUtil::KulloAddressHelper::isValidKulloDomain("192.168.178.1"), Eq(false));
}

TEST_F(KulloAddressHelper, split)
{
    QString addr1("hi#kullo.net");
    QString addr2("hi#example.com");

    EXPECT_THAT(DesktopUtil::KulloAddressHelper::splitKulloAddress(addr1), Eq(QStringList{"hi", "kullo.net"}));
    EXPECT_THAT(DesktopUtil::KulloAddressHelper::splitKulloAddress(addr2), Eq(QStringList{"hi", "example.com"}));
}
