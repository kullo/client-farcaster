/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>
#include <boost/optional.hpp>

namespace KulloDesktop {
namespace Qml {

class ModalityWatcher : public QObject
{
    Q_OBJECT

public:
    explicit ModalityWatcher(QObject *parent = nullptr);

    Q_INVOKABLE bool aboutToOpen(QString objectName);
    Q_INVOKABLE void closed(QString objectName);

private:
    boost::optional<QString> currentObjectName_;
};

}
}
