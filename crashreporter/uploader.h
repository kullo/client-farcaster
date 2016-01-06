/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
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
