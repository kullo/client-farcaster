/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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
