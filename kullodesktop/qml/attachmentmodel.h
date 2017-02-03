/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/types.h>
#include <kulloclient/api/Session.h>

namespace KulloDesktop {
namespace Qml {

class AttachmentModel : public QObject
{
    Q_OBJECT

public:
    explicit AttachmentModel(QObject *parent = nullptr);
    AttachmentModel(
            const std::shared_ptr<Kullo::Api::Session> &session,
            Kullo::id_type msgId,
            Kullo::id_type attId,
            QObject *parent = nullptr);

    Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged)
    QString filename() const;

    Q_PROPERTY(QString hash READ hash NOTIFY hashChanged)
    QString hash() const;

    Q_PROPERTY(QString mimeType READ mimeType NOTIFY mimeTypeChanged)
    QString mimeType() const;

    Q_PROPERTY(quint32 size READ size NOTIFY sizeChanged)
    quint32 size() const;

    Q_INVOKABLE bool saveTo(const QUrl &url) const;
    Q_INVOKABLE bool open() const;

    QByteArray content() const;

signals:
    void filenameChanged();
    void hashChanged();
    void mimeTypeChanged();
    void sizeChanged();

private:
    Kullo::id_type messageId() const;
    bool doSaveTo(const std::string &path) const;

    std::shared_ptr<Kullo::Api::Session> session_;
    Kullo::id_type msgId_ = -1;
    Kullo::id_type attId_ = -1;
};

}
}
