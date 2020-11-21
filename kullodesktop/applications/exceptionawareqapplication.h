/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
