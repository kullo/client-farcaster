/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <apimirror/apimirror-forwards.h>
#include <kulloclient/kulloclient-forwards.h>

namespace KulloDesktop {
namespace Qml {

class LoginChecker : public QObject
{
    Q_OBJECT

public:
    explicit LoginChecker(QObject *parent = nullptr);
    ~LoginChecker();

    Q_PROPERTY(ApiMirror::Client* client READ client WRITE setClient NOTIFY clientChanged)
    ApiMirror::Client *client();
    void setClient(ApiMirror::Client *client);

    Q_PROPERTY(bool locked READ locked NOTIFY lockedChanged)
    bool locked() const;

    Q_INVOKABLE void run(const QString &addr, const QStringList &masterKeyBlocks);

signals:
    // QML properties
    void clientChanged();
    void lockedChanged();

    // others
    void invalidKulloAddress();
    void invalidMasterKey();
    void loginChecked(const QString &address, const QString &masterKeyPem, bool ok);

    void serverError(const QString &address);
    void protocolError(const QString &address);
    void networkError(const QString &address);

private:
    void setLocked(bool locked);

    ApiMirror::Client *client_ = nullptr;
    std::shared_ptr<ApiMirror::ClientCheckCredentialsListener> listener_;
    std::shared_ptr<Kullo::Api::AsyncTask> bgTask_;
    bool locked_ = false;
};

}
}
