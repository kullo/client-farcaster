/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>
#include <QFont>

namespace KulloDesktop {
namespace Qml {

class FontList : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(QFont Monospace READ monospace NOTIFY somethingChanged)
    QFont monospace() const;

    Q_PROPERTY(QFont Serif READ serif NOTIFY somethingChanged)
    QFont serif() const;

    Q_PROPERTY(QFont SansSerif READ sansSerif NOTIFY somethingChanged)
    QFont sansSerif() const;

    Q_PROPERTY(QString MonospaceFamily READ monospaceFamily NOTIFY somethingChanged)
    QString monospaceFamily() const;

    Q_PROPERTY(QString SerifFamily READ serifFamily NOTIFY somethingChanged)
    QString serifFamily() const;

    Q_PROPERTY(QString SansSerifFamily READ sansSerifFamily NOTIFY somethingChanged)
    QString sansSerifFamily() const;

signals:
    void somethingChanged();
};

}
}
