/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include "kullodesktop/applications/kulloapplication.h"

namespace KulloDesktop {
namespace Applications {

class OsxKulloApplication : public KulloApplication
{
    Q_OBJECT

public:
    explicit OsxKulloApplication(int &argc, char **argv);

public slots:
    void onDockIconClicked();
};

}
}
