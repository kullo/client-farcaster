/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include "kullodesktop/farcaster-forwards.h"

class QQmlApplicationEngine;

namespace KulloDesktop {
namespace QmlBridge {

class QmlSetup
{
public:
    static void setupTypes();
    static void setupImageproviders(QQmlApplicationEngine &engine, Qml::Inbox &clientModel);
};

}
}
