/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <thread>
#include <mutex>
#include <boost/optional.hpp>
#include <kulloclient/http/HttpClient.h>
#include <kulloclient/http/Response.h>
#include <kulloclient/http/ResponseError.h>

namespace DesktopUtil {

/*
 * Limitations:
 * - POST only
 * - Only one request at a time
 * - Request body in memory entirely
 * - Ignores response body
 * - No progress signals
 * - No cancel
 * - Chunked encoding is disabled
 */
class AsyncHttpPostManager : public QObject
{
    Q_OBJECT

public:
    using Response = boost::optional<Kullo::Http::Response>;
    using ResponseError = Kullo::Http::ResponseError;

    explicit AsyncHttpPostManager(QObject *parent = 0);
    ~AsyncHttpPostManager();
    void post(const QUrl &url, const std::string &boundary, const std::vector<uint8_t> &data);

signals:
    void finished(Response response);

    // internal
    void _bgJobfinished(const Response &response);

private slots:
    void onBgJobFinished(const Response &response);

private:
    std::shared_ptr<Kullo::Http::HttpClient> client_;
    std::thread backgroundJob_;
    std::mutex running_;
};

}
