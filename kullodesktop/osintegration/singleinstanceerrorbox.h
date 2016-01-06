/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QMessageBox>

namespace KulloDesktop {
namespace OsIntegration {

class SingleInstanceErrorBox : public QMessageBox
{
    Q_OBJECT

public:
    SingleInstanceErrorBox(const QString &appName, QWidget *parent = 0);
};

}
}
