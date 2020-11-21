/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
