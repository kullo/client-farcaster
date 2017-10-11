/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "kulloaddresshelper.h"

#include <QStringList>

#include <kulloclient/api/AddressHelpers.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/util/assert.h>

namespace DesktopUtil {

bool KulloAddressHelper::isValidKulloAddress(const QString &address)
{
    return !!Kullo::Api::AddressHelpers::create(address.toStdString());
}

bool KulloAddressHelper::isValidKulloDomain(const QString &domain)
{
    const QString testAddress = "test#" + domain;
    return isValidKulloAddress(testAddress);
}

bool KulloAddressHelper::kulloAddressEqual(const QString &lhs, const QString &rhs)
{
    auto address1 = *Kullo::Api::AddressHelpers::create(lhs.toStdString());
    auto address2 = *Kullo::Api::AddressHelpers::create(rhs.toStdString());
    return address1 == address2;
}

QStringList KulloAddressHelper::splitKulloAddress(const QString &address)
{
    kulloAssert(isValidKulloAddress(address));

    return address.split('#');
}

}
