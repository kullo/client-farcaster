/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
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

    virtual bool notify(QObject *receiver, QEvent *event);
};

}
}
