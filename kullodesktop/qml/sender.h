/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/types.h>

namespace KulloDesktop {
namespace Qml {

class Sender : public QObject
{
    Q_OBJECT

public:
    explicit Sender(QObject *parent = nullptr);
    explicit Sender(const std::shared_ptr<Kullo::Api::Session> &session,
                    Kullo::id_type msgId,
                    QObject *parent = nullptr);

    Q_PROPERTY(QString name READ name)
    QString name() const;

    Q_PROPERTY(QString address READ address)
    QString address() const;

    Q_PROPERTY(QString organization READ organization)
    QString organization() const;

    // Non-QML

    QString avatarMimeType() const;
    QByteArray avatar() const;

private:
    std::shared_ptr<Kullo::Api::Session> session_;
    Kullo::id_type msgId_ = -1;
};

}
}
