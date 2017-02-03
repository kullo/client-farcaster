/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include <iostream>

#include <httpclient/httpclientfactoryimpl.h>

#include <kulloclient/api/Address.h>
#include <kulloclient/api/MasterKey.h>
#include <kulloclient/api/Registry.h>
#include <kulloclient/http/Registry.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/stltaskrunner.h>

#include "registerer.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./registerer <address>" << std::endl;
        std::exit(1);
    }

    const auto adressString = std::string(argv[1]);
    auto address = Kullo::Api::Address::create(adressString);
    if (!address)
    {
        Log.e() << "Invalid adress format: " << adressString;
        return 1;
    }

    auto taskRunner = std::make_shared<Kullo::Util::StlTaskRunner>();
    Kullo::Api::Registry::setTaskRunner(taskRunner);

    Kullo::Http::Registry::setHttpClientFactory(
                std::shared_ptr<Kullo::Http::HttpClientFactory>(
                    new HttpClient::HttpClientFactoryImpl()));

    Log.i() << "Registering " << address->toString();
    auto masterKey = Registerer().run(address);
    kulloAssert(masterKey);
    std::cout << masterKey->pem();

    taskRunner->wait();
    return 0;
}
