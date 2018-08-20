/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "asynchttppostmanager.h"

#include <QUrl>
#include <httpclient/httpclientfactoryimpl.h>
#include <kulloclient/http/HttpMethod.h>
#include <kulloclient/http/Request.h>
#include <kulloclient/http/RequestListener.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "desktoputil/asynctask.h"
#include "desktoputil/qtypestreamers.h"

namespace {

class RequestListener : public Kullo::Http::RequestListener
{
public:
    RequestListener(const std::vector<uint8_t> &bodyData)
        : bodyData_(bodyData)
    {
    }

    std::vector<uint8_t> read(int64_t maxSizeSigned)
    {
        kulloAssert(maxSizeSigned >= 0);
        // We assume that maxSizeSigned <= max(size_t). This is a valid
        // assumption because otherwise the maximum would be larger than the
        // address space...
        auto maxSize = static_cast<size_t>(maxSizeSigned);

        kulloAssert(bodyData_.begin() + reqBodyBytesRead_ <= bodyData_.end());
        auto chunkSize = std::min(bodyData_.size() - reqBodyBytesRead_, maxSize);
        std::vector<uint8_t> result;
        result.reserve(chunkSize);
        std::copy(bodyData_.begin() + reqBodyBytesRead_,
                  bodyData_.begin() + reqBodyBytesRead_ + chunkSize,
                  std::back_inserter(result));
        reqBodyBytesRead_ += chunkSize;
        return result;
    }
private:
    std::vector<uint8_t> bodyData_;
    size_t reqBodyBytesRead_ = 0;
};

}

namespace DesktopUtil {

AsyncHttpPostManager::AsyncHttpPostManager(QObject *parent)
    : QObject(parent)
    , client_(HttpClient::HttpClientFactoryImpl().createHttpClient())
{
    qRegisterMetaType<Response>("Response");

    connect(this, &AsyncHttpPostManager::_bgJobfinished,
            this, &AsyncHttpPostManager::onBgJobFinished);
}

AsyncHttpPostManager::~AsyncHttpPostManager()
{
    if (backgroundJob_.joinable())
    {
        backgroundJob_.join();
    }
}

void AsyncHttpPostManager::post(const QUrl &url, const std::string &boundary, const std::vector<uint8_t> &data)
{
    running_.lock();
    if (backgroundJob_.joinable()) backgroundJob_.join();

    Log.i() << "POST " << url;

    auto headers = std::vector<Kullo::Http::HttpHeader>{};
    headers.emplace_back("Content-Type", "multipart/form-data; boundary=\"" + boundary +  "\"");
    headers.emplace_back("Expect", "");
    headers.emplace_back("Transfer-Encoding", "");
    headers.emplace_back("Content-Length", std::to_string(data.size()));

    auto request = Kullo::Http::Request(Kullo::Http::HttpMethod::Post,
                                          url.toString().toStdString(),
                                          headers);

    auto reqL = std::make_shared<RequestListener>(data);

    backgroundJob_ = AsyncTask::run([this, request, reqL]()
    {
        Kullo::Util::LibraryLogger::setCurrentThreadName("POST");
        boost::optional<Kullo::Http::Response> response;
        response = boost::make_optional<Kullo::Http::Response>(
                    client_->sendRequest(request, 0, reqL, nullptr)
                    );

        emit _bgJobfinished(response);
    });
}

void AsyncHttpPostManager::onBgJobFinished(const AsyncHttpPostManager::Response &response)
{
    kulloAssert(response);
    running_.unlock();
    emit finished(response);
}

}
