/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QObject>
#include <QPixmap>
#include <desktoputil/dice/dice-forwards.h>

namespace KulloDesktop {
namespace Qml {

class ParticipantModel : public QObject
{
    Q_OBJECT

public:
    explicit ParticipantModel(QObject *parent = 0);
    explicit ParticipantModel(std::shared_ptr<Kullo::Model::Participant> part, QObject *parent);

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    QString name() const;

    Q_PROPERTY(QString address READ address NOTIFY addressChanged)
    QString address() const;

    Q_PROPERTY(QString organization READ organization NOTIFY organizationChanged)
    QString organization() const;

    Q_PROPERTY(QPixmap avatar READ avatar NOTIFY avatarChanged)
    QPixmap avatar() const;

signals:
    void nameChanged();
    void addressChanged();
    void organizationChanged();
    void avatarChanged();

private:
    std::shared_ptr<Kullo::Model::Participant> part_;
};

}
}
