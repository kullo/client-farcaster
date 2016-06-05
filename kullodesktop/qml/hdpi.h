/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

namespace KulloDesktop {
namespace Qml {

class Hdpi : public QObject
{
    Q_OBJECT

public:
    explicit Hdpi(QObject *parent = 0);

    Q_PROPERTY(qreal FontScalingFactor READ fontScalingFactor NOTIFY somethingChanged)
    qreal fontScalingFactor() const;

signals:
    void somethingChanged();
};

}
}
