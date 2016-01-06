/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <kulloclient/kulloclient-forwards.h>
#include <desktoputil/dice/dice-forwards.h>

namespace KulloDesktop {
namespace Qml {

class AttachmentModel : public QObject
{
    Q_OBJECT

public:
    explicit AttachmentModel(QObject *parent = 0);
    explicit AttachmentModel(Kullo::Model::Attachment *att, QObject *parent);

    Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged)
    QString filename() const;

    Q_PROPERTY(QString mimeType READ mimeType NOTIFY mimeTypeChanged)
    QString mimeType() const;

    Q_PROPERTY(quint32 size READ size NOTIFY sizeChanged)
    quint32 size() const;

    Q_PROPERTY(QString note READ note NOTIFY noteChanged)
    QString note() const;

    Q_INVOKABLE bool saveTo(const QUrl &url) const;
    Q_INVOKABLE bool open() const;

    QByteArray content() const;

signals:
    void filenameChanged();
    void mimeTypeChanged();
    void sizeChanged();
    void noteChanged();

private:
    quint32 messageId() const;

    Kullo::Model::Attachment *attachment_ = nullptr;
};

}
}
