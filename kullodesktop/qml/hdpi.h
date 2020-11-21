/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

namespace KulloDesktop {
namespace Qml {

class Hdpi : public QObject
{
    Q_OBJECT

public:
    explicit Hdpi(QObject *parent = nullptr);

    Q_PROPERTY(qreal FontScalingFactor READ fontScalingFactor NOTIFY somethingChanged)
    qreal fontScalingFactor() const;

    Q_PROPERTY(qreal MaxDevicePixelRatio READ maxDevicePixelRatio NOTIFY somethingChanged)
    qreal maxDevicePixelRatio() const;

signals:
    void somethingChanged();
};

}
}
