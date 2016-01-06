/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <desktoputil/dice/dice-forwards.h>

namespace KulloDesktop {
namespace Qml {

class DraftAttachmentModel : public QObject
{
    Q_OBJECT

public:
    explicit DraftAttachmentModel(QObject *parent = 0);
    explicit DraftAttachmentModel(Kullo::Model::DraftAttachment *att, QObject *parent);

    Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged)
    QString filename() const;

    Q_PROPERTY(QString index READ index)
    quint32 index() const;

    Q_PROPERTY(QString mimeType READ mimeType NOTIFY mimeTypeChanged)
    QString mimeType() const;

    Q_PROPERTY(QString hash READ hash NOTIFY hashChanged)
    QString hash() const;

    Q_PROPERTY(quint32 size READ size NOTIFY sizeChanged)
    quint32 size() const;

    Q_PROPERTY(QString note READ note WRITE setNote NOTIFY noteChanged)
    QString note() const;
    void setNote(QString note);

    Q_INVOKABLE void deletePermanently();
    Q_INVOKABLE bool open();

    QByteArray content() const;

signals:
    /// Emitted when (potentially) everything has changed and should be reloaded
    void changed();

    void filenameChanged();
    void mimeTypeChanged();
    void hashChanged();
    void sizeChanged();
    void noteChanged();

private:
    void save();

    Kullo::Model::DraftAttachment *draftAttachment_ = nullptr;
};

}
}
