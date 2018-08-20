/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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
