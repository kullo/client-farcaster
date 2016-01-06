/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <apimirror/apimirror-forwards.h>
#include <kulloclient/kulloclient-forwards.h>

namespace KulloDesktop {
namespace Qml {

class ExistenceChecker : public QObject
{
    Q_OBJECT

public:
    explicit ExistenceChecker(QObject *parent = nullptr);
    ~ExistenceChecker();

    Q_PROPERTY(ApiMirror::Client* client READ client WRITE setClient NOTIFY clientChanged)
    ApiMirror::Client *client();
    void setClient(ApiMirror::Client *client);

    Q_PROPERTY(bool locked READ locked NOTIFY lockedChanged)
    bool locked() const;

    Q_INVOKABLE bool checkExistence(const QString &addr);

signals:
    // QML properties
    void clientChanged();
    void lockedChanged();

    // others
    void invalidKulloAddress();
    void existenceChecked(const QString &address, bool ok);

    void networkError(const QString &address);

    // TODO: errors in UI
    void serverError(const QString &address);
    void protocolError(const QString &address);

private slots:
    void onExistenceChecked(const std::string &address, bool ok);
    void onError(const std::string &address, Kullo::Api::NetworkError error);

private:
    void setLocked(bool locked);

    ApiMirror::Client *client_ = nullptr;
    std::shared_ptr<ApiMirror::ClientAddressExistsListener> listener_;
    std::shared_ptr<Kullo::Api::AsyncTask> bgTask_;
    bool locked_ = false;
};

}
}
