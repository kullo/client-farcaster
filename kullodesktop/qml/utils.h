/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QColor>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include <desktoputil/kulloaddresshelper.h>
#include <kulloclient/util/masterkey.h>

namespace KulloDesktop {
namespace Qml {

class Utils : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE bool isPlausibleSingleBlock(const QString &address) const
        { return Kullo::Util::MasterKey::isPlausibleSingleBlock(address.toStdString()); }
    Q_INVOKABLE bool isValidKulloAddress(const QString &address) const
        { return DesktopUtil::KulloAddressHelper::isValidKulloAddress(address); }
    Q_INVOKABLE bool isValidKulloDomain(const QString &domain) const
        { return DesktopUtil::KulloAddressHelper::isValidKulloDomain(domain); }
    Q_INVOKABLE bool kulloAddressEqual(const QString &lhs, const QString &rhs) const
        { return DesktopUtil::KulloAddressHelper::kulloAddressEqual(lhs, rhs); }
    Q_INVOKABLE QStringList splitKulloAddress(const QString &address) const
        { return DesktopUtil::KulloAddressHelper::splitKulloAddress(address); }

    Q_INVOKABLE QStringList extractDataFromPemMasterKey(const QString &key) const;
    Q_INVOKABLE bool isValidPemMasterKey(const QString &key) const;

    Q_INVOKABLE void setClipboardText(QString txt) const;
    Q_INVOKABLE QUrl makeFileUrl(const QString &path) const;
    Q_INVOKABLE QColor setSaturation(const QColor &in, const qreal saturation) const;
    Q_INVOKABLE QColor setAlpha(const QColor &in, const qreal alpha) const;
    Q_INVOKABLE QString urlencode(const QString &in) const;
    Q_INVOKABLE QUrl defaultSaveAttachmentsDir() const;
};

}
}
