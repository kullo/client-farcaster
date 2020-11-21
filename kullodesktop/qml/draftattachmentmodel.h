/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <functional>
#include <memory>

#include <QObject>

#include <apimirror/apimirror-forwards.h>
#include <kulloclient/types.h>
#include <kulloclient/kulloclient-forwards.h>

namespace KulloDesktop {
namespace Qml {

class DraftAttachmentModel : public QObject
{
    Q_OBJECT

public:
    explicit DraftAttachmentModel(QObject *parent = nullptr);
    DraftAttachmentModel(
            const std::shared_ptr<Kullo::Api::Session> &session,
            Kullo::id_type convId,
            Kullo::id_type attId,
            QObject *parent = nullptr);

    Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged)
    QString filename() const;

    Q_PROPERTY(Kullo::id_type index READ index)
    Kullo::id_type index() const;

    Q_PROPERTY(QString mimeType READ mimeType NOTIFY mimeTypeChanged)
    QString mimeType() const;

    Q_PROPERTY(QString hash READ hash NOTIFY hashChanged)
    QString hash() const;

    Q_PROPERTY(Kullo::id_type size READ size NOTIFY sizeChanged)
    Kullo::id_type size() const;

    Q_INVOKABLE void deletePermanently();

    // returns true if async saving/opening started successfully. Error might occur during operation
    Q_INVOKABLE bool openAsync();

signals:
    void filenameChanged();
    void mimeTypeChanged();
    void hashChanged();
    void sizeChanged();

private:
    void pruneDoneTasks();
    bool doSaveTo(const std::string &path, const std::function<void (bool)> &callback);

    std::shared_ptr<Kullo::Api::Session> session_;
    Kullo::id_type convId_ = -1;
    Kullo::id_type attId_ = -1;
    std::shared_ptr<Kullo::Api::AsyncTask> saveToTask_;
    std::shared_ptr<ApiMirror::DraftAttachmentsSaveToListener> saveToListener_;
};

}
}
