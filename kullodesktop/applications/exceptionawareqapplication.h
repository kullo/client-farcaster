/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <QApplication>

namespace KulloDesktop {
namespace Applications {

class ExceptionAwareQApplication : public QApplication
{
    Q_OBJECT

public:
    explicit ExceptionAwareQApplication(int &argc, char **argv);
    virtual ~ExceptionAwareQApplication() = default;

    bool notify(QObject *receiver, QEvent *event) override;
};

}
}
