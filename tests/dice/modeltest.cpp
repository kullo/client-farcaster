/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "modeltest.h"

#include <kulloclient/util/kulloaddress.h>
#include <kulloclient/util/misc.h>

using namespace Kullo;

std::unique_ptr<Model::Client> ClientBuilder::build(const Kullo::Db::SharedSessionPtr &session)
{
    auto client = make_unique<Kullo::Model::Client>();
    Kullo::Util::UserSettings userSettings;
    userSettings.address.reset(new Kullo::Util::KulloAddress("localtestuser#kullo.net"));
    client->userSettings() = userSettings;
    client->logIn(session);
    return client;
}

void ModelTest::SetUp()
{
    DbTest::SetUp();
    client_ = ClientBuilder::build(session_);
}
