/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "integrationtest.h"

#include <kulloclient/api/Registry.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/http/HttpClient.h>
#include <kulloclient/http/HttpClientFactory.h>
#include <kulloclient/http/HttpMethod.h>
#include <kulloclient/http/Registry.h>
#include <kulloclient/http/Request.h>
#include <kulloclient/http/Response.h>
#include <kulloclient/registry.h>

#include <httpclient/httpclientfactoryimpl.h>

using namespace Kullo;
using namespace testing;

namespace testing {

MATCHER_P(HttpHeaderEq, rhs, std::string(negation ? "isn't" : "is") + " " + PrintToString(rhs)) {
    auto lhs = arg;
    return lhs.key == rhs.key && lhs.value == rhs.value;
}

} // testing

namespace Kullo { namespace Http {

std::ostream &operator<<(std::ostream &out, const Http::HttpHeader &header)
{
    out << header.key << ": " << header.value;
    return out;
}

} } // Kullo::Http

class Networking : public IntegrationTest
{
public:
    Networking() {
        auto httpClientFactory = std::make_shared<HttpClient::HttpClientFactoryImpl>();
        Registry::setHttpClientFactory(httpClientFactory);
        httpClient_ = Registry::httpClientFactory()->createHttpClient().as_nullable();
    }

protected:
    std::shared_ptr<Http::HttpClient> httpClient_;
    int timeoutMs_ = 60*1000;
};

TEST_F(Networking, getHttp)
{
    auto request = Http::Request(Http::HttpMethod::Get, "http://www.httpvshttps.com/", {});
    auto response = httpClient_->sendRequest(request, timeoutMs_, nullptr, nullptr);
    EXPECT_THAT(response.error, Eq(boost::none));
    EXPECT_THAT(response.statusCode, Eq(200));
    EXPECT_THAT(response.headers.size(), Ge(2u));
    EXPECT_THAT(response.headers, Contains(HttpHeaderEq(Http::HttpHeader{"Content-Type", "text/html; charset=UTF-8"})));
}

TEST_F(Networking, getHttps)
{
    auto request = Http::Request(Http::HttpMethod::Get, "https://www.httpvshttps.com/", {});
    auto response = httpClient_->sendRequest(request, timeoutMs_, nullptr, nullptr);
    EXPECT_THAT(response.error, Eq(boost::none));
    EXPECT_THAT(response.statusCode, Eq(200));
    EXPECT_THAT(response.headers.size(), Ge(2u));
    EXPECT_THAT(response.headers, Contains(HttpHeaderEq(Http::HttpHeader{"Content-Type", "text/html; charset=UTF-8"})));
}

TEST_F(Networking, multipleTlsCertificates)
{
    const auto urls = std::vector<std::string> {
            "https://www.kullo.net/en/",
            "https://kullo.kullo.net/v1/status",
            "https://de.wikipedia.org/wiki/Wikipedia:Hauptseite",
            "https://www.cloudflare.com/",
            "https://stackoverflow.com/"
    };

    for (const auto url : urls)
    {
        auto request = Http::Request(Http::HttpMethod::Get, url, {});
        auto response = httpClient_->sendRequest(request, timeoutMs_, nullptr, nullptr);
        EXPECT_THAT(response.error, Eq(boost::none));
        EXPECT_THAT(response.statusCode, Eq(200));
        EXPECT_THAT(response.headers.size(), Ge(2u));
    }
}
