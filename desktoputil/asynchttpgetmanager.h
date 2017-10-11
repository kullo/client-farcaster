/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QIODevice>
#include <QObject>
#include <QUrl>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

#include <boost/optional.hpp>
#include <kulloclient/http/HttpClient.h>
#include <kulloclient/http/HttpHeader.h>
#include <kulloclient/http/Response.h>

namespace DesktopUtil {

/*
 * Limitations:
 * - GET only
 * - Only one request at a time
 * - Ignores response body
 */
class AsyncHttpGetManager : public QObject
{
    Q_OBJECT

public:
    using Response = boost::optional<Kullo::Http::Response>;
    using ResponseError = Kullo::Http::ResponseError;

    explicit AsyncHttpGetManager(QObject *parent = 0);
    ~AsyncHttpGetManager();

    void get(const QUrl &url, std::shared_ptr<QIODevice> target);
    void cancel();

signals:
    void downloadProgress(quint64 bytesReceived, quint64 bytesTotal);
    void finished(Response response);

    // internal
    void _bgJobfinished(const Response &response);

private slots:
    void onBgJobFinished(const Response &response);

private:
    Kullo::nn_shared_ptr<Kullo::Http::HttpClient> client_;
    std::atomic<bool> canceled_;
    std::shared_ptr<QIODevice> target_;
    std::thread backgroundJob_;
    std::mutex running_;
};

}
