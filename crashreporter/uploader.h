/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>
#include <desktoputil/asynchttppostmanager.h>

#include "crashreporter/metadata.h"

class Uploader : public QObject
{
    Q_OBJECT

public:
    explicit Uploader(QObject *parent = nullptr);
    ~Uploader();
    void upload(const Metadata &meta);

signals:
    void finished(bool ok, QString errorMessage = "");

private slots:
    void onFinished(DesktopUtil::AsyncHttpPostManager::Response response);

private:
    DesktopUtil::AsyncHttpPostManager manager_;
};
