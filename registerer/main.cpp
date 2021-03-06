/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include <iostream>

#include <httpclient/httpclientfactoryimpl.h>

#include <kulloclient/api/AddressHelpers.h>
#include <kulloclient/api/MasterKeyHelpers.h>
#include <kulloclient/api/Registry.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/api_impl/MasterKey.h>
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
    auto address = Kullo::Api::AddressHelpers::create(adressString);
    if (!address)
    {
        Log.e() << "Invalid adress format: " << adressString;
        return 1;
    }

    auto taskRunner = Kullo::nn_make_shared<Kullo::Util::StlTaskRunner>();
    Kullo::Api::Registry::setTaskRunner(taskRunner);

    Kullo::Http::Registry::setHttpClientFactory(
                kulloForcedNn(std::shared_ptr<Kullo::Http::HttpClientFactory>(
                                  new HttpClient::HttpClientFactoryImpl())));

    Log.i() << "Registering " << address->toString();
    auto masterKey = Registerer().run(*address);
    std::cout << Kullo::Api::MasterKeyHelpers::toPem(masterKey);

    taskRunner->wait();
    return 0;
}
