/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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

    explicit AsyncHttpPostManager(QObject *parent = nullptr);
    ~AsyncHttpPostManager();
    void post(const QUrl &url, const std::string &boundary, const std::vector<uint8_t> &data);

signals:
    void finished(Response response);

    // internal
    void _bgJobfinished(const Response &response);

private slots:
    void onBgJobFinished(const Response &response);

private:
    Kullo::nn_shared_ptr<Kullo::Http::HttpClient> client_;
    std::thread backgroundJob_;
    std::mutex running_;
};

}
