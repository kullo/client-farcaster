/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <atomic>
#include <memory>
#include <QObject>
#include <desktoputil/dice/model/accountinfo.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class SettingsLocationModel : public QObject
{
    Q_OBJECT

public:
    explicit SettingsLocationModel(QObject *parent = 0);
    ~SettingsLocationModel();

    Q_PROPERTY(KulloDesktop::Qml::UserSettingsModel *userSettings READ userSettings WRITE setUserSettings NOTIFY userSettingsChanged)
    UserSettingsModel *userSettings() const;
    void setUserSettings(UserSettingsModel *settings);

    Q_INVOKABLE void openUrl();

signals:
    void userSettingsChanged();

private:
    std::atomic<bool> locked_;
    std::unique_ptr<Kullo::Model::AccountInfo> accountInfo_;
    UserSettingsModel *userSettings_;
};

}
}
