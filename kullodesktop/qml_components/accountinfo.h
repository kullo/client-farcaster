/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <atomic>
#include <memory>
#include <QObject>

#include <kulloclient/kulloclient-forwards.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace QmlComponents {

class AccountInfo : public QObject
{
    Q_OBJECT

public:
    explicit AccountInfo(QObject *parent = nullptr);
    ~AccountInfo();

    Q_PROPERTY(KulloDesktop::Qml::Inbox* inbox READ inbox WRITE setInbox NOTIFY inboxChanged)
    Qml::Inbox *inbox();
    void setInbox(Qml::Inbox *inbox);

    Q_PROPERTY(QString planName READ planName NOTIFY planNameChanged)
    QString planName() const;

    // note: <int64> aka. <long> support is not great in QML. We cannot assign <long> to a <real> property
    Q_PROPERTY(qreal storageQuota READ storageQuota NOTIFY storageQuotaChanged)
    qreal storageQuota() const;

    Q_PROPERTY(qreal storageUsed READ storageUsed NOTIFY storageUsedChanged)
    qreal storageUsed() const;

    Q_INVOKABLE void openSettingsLocationUrl();

    Q_INVOKABLE void reload();

signals:
    void inboxChanged();
    void planNameChanged();
    void storageQuotaChanged();
    void storageUsedChanged();

private slots:
    void onNewAccountInfoReceived(const std::shared_ptr<Kullo::Api::AccountInfo> accountInfo);

private:
    Qml::Inbox *inbox_ = nullptr;
    std::atomic<bool> locked_;
    std::shared_ptr<Kullo::Api::AsyncTask> task_;
    std::shared_ptr<Kullo::Api::AccountInfo> accountInfo_;
};

}
}
