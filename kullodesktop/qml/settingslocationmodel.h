/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <atomic>
#include <memory>
#include <QObject>

#include <kulloclient/api/AsyncTask.h>

#include "kullodesktop/farcaster-forwards.h"
#include "kullodesktop/qml/clientmodel.h"

namespace KulloDesktop {
namespace Qml {

class SettingsLocationModel : public QObject
{
    Q_OBJECT

public:
    explicit SettingsLocationModel(QObject *parent = nullptr);
    ~SettingsLocationModel();

    Q_INVOKABLE void openUrl(KulloDesktop::Qml::ClientModel *clientModel);

private:
    std::atomic<bool> locked_;
    std::shared_ptr<Kullo::Api::AsyncTask> task_;
};

}
}
