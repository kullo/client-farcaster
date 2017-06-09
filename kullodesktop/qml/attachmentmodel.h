/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <functional>
#include <memory>

#include <QObject>

#include <apimirror/apimirror-forwards.h>
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

    // returns true if async saving started successfully. Error might occur during operation
    Q_INVOKABLE bool saveToAsync(const QUrl &url);

    // returns true if async saving/opening started successfully. Error might occur during operation
    Q_INVOKABLE bool openAsync();

signals:
    void filenameChanged();
    void hashChanged();
    void mimeTypeChanged();
    void sizeChanged();

private:
    Kullo::id_type messageId() const;
    void pruneDoneTasks();
    bool doSaveTo(const std::string &path, const std::function<void (bool)> &callback);

    std::shared_ptr<Kullo::Api::Session> session_;
    Kullo::id_type msgId_ = -1;
    Kullo::id_type attId_ = -1;
    std::shared_ptr<Kullo::Api::AsyncTask> saveToTask_;
    std::shared_ptr<ApiMirror::MessageAttachmentsSaveToListener> saveToListener_;
};

}
}
