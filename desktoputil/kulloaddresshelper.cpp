/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "kulloaddresshelper.h"

#include <QStringList>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/kulloaddress.h>

namespace DesktopUtil {

bool KulloAddressHelper::isValidKulloAddress(const QString &address)
{
    return Kullo::Util::KulloAddress::isValid(address.toStdString());
}

bool KulloAddressHelper::isValidKulloDomain(const QString &domain)
{
    const QString testAddress = "test#" + domain;
    return isValidKulloAddress(testAddress);
}

bool KulloAddressHelper::kulloAddressEqual(const QString &lhs, const QString &rhs)
{
    return Kullo::Util::KulloAddress(lhs.toStdString())
            == Kullo::Util::KulloAddress(rhs.toStdString());
}

QStringList KulloAddressHelper::splitKulloAddress(const QString &address)
{
    kulloAssert(isValidKulloAddress(address));

    return address.split('#');
}

}
