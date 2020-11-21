/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <QStringList>

namespace KulloDesktop {
namespace Qml {

class SoftwareVersions : public QObject
{
    Q_OBJECT

public:
    explicit SoftwareVersions(QObject *parent = nullptr);

    Q_PROPERTY(QStringList list READ list NOTIFY listChanged)
    static QStringList list();

    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    static QString version();

    Q_PROPERTY(QString versionSimplified READ versionSimplified NOTIFY versionSimplifiedChanged)
    static QString versionSimplified();

signals:
    // unused signals
    void listChanged();
    void versionChanged();
    void versionSimplifiedChanged();
};

}
}
