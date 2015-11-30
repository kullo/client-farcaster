/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include <QCoreApplication>

#include <iostream>

#include <httpclient/httpclientfactoryimpl.h>

#include <desktoputil/metatypes.h>
#include <desktoputil/dice/dice-forwards.h>
#include <desktoputil/dice/model/registration.h>

#include <kulloclient/http/Registry.h>
#include <kulloclient/util/formatstring.h>
#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/masterkey.h>

#include "registerer.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: ./registerer <address> <masterKeyFile>" << std::endl;
        std::exit(1);
    }

    DesktopUtil::registerMetaTypes();
    Kullo::Http::Registry::setHttpClientFactory(
                std::shared_ptr<Kullo::Http::HttpClientFactory>(
                    new HttpClient::HttpClientFactoryImpl()));

    QCoreApplication app(argc, argv);

    auto address = Kullo::Util::KulloAddress(std::string(argv[1]));
    auto masterKey = Kullo::Util::MasterKey(Registerer::getStringContentsOrCrash(std::string(argv[2])));

    Log.i() << "Registering " << address;

    Registerer registerer(app);
    registerer.run(address, masterKey);

    return app.exec();
}
