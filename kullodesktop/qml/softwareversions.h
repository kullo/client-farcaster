/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <QStringList>

namespace KulloDesktop {
namespace Qml {

class SoftwareVersions : public QObject
{
    Q_OBJECT

public:
    explicit SoftwareVersions(QObject *parent = 0);

    Q_PROPERTY(QStringList list READ list NOTIFY listChanged)
    static QStringList list();

    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    static QString version();

signals:
    // unused signals
    void listChanged();
    void versionChanged();
};

}
}
