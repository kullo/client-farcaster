/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "kulloaddresshelper.h"

#include <QStringList>

#include <kulloclient/api/Address.h>
#include <kulloclient/util/assert.h>

namespace DesktopUtil {

bool KulloAddressHelper::isValidKulloAddress(const QString &address)
{
    return !!Kullo::Api::Address::create(address.toStdString());
}

bool KulloAddressHelper::isValidKulloDomain(const QString &domain)
{
    const QString testAddress = "test#" + domain;
    return isValidKulloAddress(testAddress);
}

bool KulloAddressHelper::kulloAddressEqual(const QString &lhs, const QString &rhs)
{
    return Kullo::Api::Address::create(lhs.toStdString())->isEqualTo(
                Kullo::Api::Address::create(rhs.toStdString()));
}

QStringList KulloAddressHelper::splitKulloAddress(const QString &address)
{
    kulloAssert(isValidKulloAddress(address));

    return address.split('#');
}

}
