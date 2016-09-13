/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <atomic>
#include <memory>
#include <QObject>

#include <kulloclient/api/AsyncTask.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace QmlComponents {

class SettingsLocation : public QObject
{
    Q_OBJECT

public:
    explicit SettingsLocation(QObject *parent = nullptr);
    ~SettingsLocation();

    Q_INVOKABLE void openUrl(KulloDesktop::Qml::Inbox *inbox);

private:
    std::atomic<bool> locked_;
    std::shared_ptr<Kullo::Api::AsyncTask> task_;
};

}
}
