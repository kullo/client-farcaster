/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "asynchttpgetmanager.h"

#include <httpclient/httpclientfactoryimpl.h>
#include <kulloclient/http/HttpMethod.h>
#include <kulloclient/http/ProgressResult.h>
#include <kulloclient/http/Request.h>
#include <kulloclient/http/ResponseListener.h>
#include <kulloclient/http/TransferProgress.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#include "desktoputil/asynctask.h"
#include "desktoputil/qtypestreamers.h"

namespace {

class ResponseListener : public Kullo::Http::ResponseListener
{
public:
    using DataReceivedHandler = std::function<void(const std::vector<uint8_t> &)>;
    using ProgressHandler = std::function<Kullo::Http::ProgressResult(int64_t,int64_t)>;

    ResponseListener(const DataReceivedHandler &dataReceivedHandler, const ProgressHandler &progressHandler)
        : Kullo::Http::ResponseListener(),
          dataReceivedHandler_(dataReceivedHandler),
          progressHandler_(progressHandler)
    {
    }

    Kullo::Http::ProgressResult progressed(
            const Kullo::Http::TransferProgress &progress) override
    {
        return progressHandler_(
                    progress.downloadTransferred, progress.downloadTotal);
    }

    void dataReceived(const std::vector<uint8_t> &data) override
    {
        dataReceivedHandler_(data);
    }

private:
    DataReceivedHandler dataReceivedHandler_;
    ProgressHandler progressHandler_;
};

}

namespace DesktopUtil {

AsyncHttpGetManager::AsyncHttpGetManager(QObject *parent)
    : QObject(parent)
    , client_(HttpClient::HttpClientFactoryImpl().createHttpClient())
{
    qRegisterMetaType<Response>("Response");

    connect(this, &AsyncHttpGetManager::_bgJobfinished,
            this, &AsyncHttpGetManager::onBgJobFinished);
}

AsyncHttpGetManager::~AsyncHttpGetManager()
{
    cancel();
    if (backgroundJob_.joinable())
    {
        backgroundJob_.join();
    }
}

void AsyncHttpGetManager::get(
        const QUrl &url, std::shared_ptr<QIODevice> target)
{
    running_.lock();
    if (backgroundJob_.joinable()) backgroundJob_.join();

    canceled_ = false;
    target_ = target;

    Log.i() << "GET " << url;

    auto request = Kullo::Http::Request(Kullo::Http::HttpMethod::Get,
                                          url.toString().toStdString(),
                                          std::vector<Kullo::Http::HttpHeader>());

    auto dataReceivedHandler = [this](const std::vector<uint8_t> &chunk)
    {
        auto bytes = chunk.size();
        auto written = target_->write(reinterpret_cast<const char *>(chunk.data()), bytes);

        kulloAssert(written >= 0);

        // written >= 0, thus cast to unsigned is safe
        kulloAssert(static_cast<size_t>(written) == bytes);
    };
    auto progressHandler = [this](
            int64_t downloadTransferred,
            int64_t downloadTotal) -> Kullo::Http::ProgressResult
    {
        kulloAssert(downloadTransferred >= 0);
        kulloAssert(downloadTotal >= 0);

        emit downloadProgress(downloadTransferred, downloadTotal);

        if (canceled_) return Kullo::Http::ProgressResult::Cancel;
        else return Kullo::Http::ProgressResult::Continue;
    };

    auto respL = std::make_shared<ResponseListener>(dataReceivedHandler,
                                                    progressHandler);

    backgroundJob_ = AsyncTask::run([this, request, respL]()
    {
        Kullo::Util::LibraryLogger::setCurrentThreadName("bgjob");
        boost::optional<Kullo::Http::Response> response;
        response = boost::make_optional<Kullo::Http::Response>(
                    client_->sendRequest(request, 0, nullptr, respL)
                    );

        emit _bgJobfinished(response);
    });
}

void AsyncHttpGetManager::cancel()
{
    canceled_ = true;
}

void AsyncHttpGetManager::onBgJobFinished(const Response &response)
{
    kulloAssert(response);
    running_.unlock();
    emit finished(response);
}

}
