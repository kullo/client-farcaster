/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <tests/dbtest.h>
#include <desktoputil/dice/model/client.h>

class ClientBuilder
{
public:
    static std::unique_ptr<Kullo::Model::Client> build(const Kullo::Db::SharedSessionPtr &session);
};

/// A DbTest with Client available
class ModelTest : public DbTest
{
public:
    std::unique_ptr<Kullo::Model::Client> client_;

protected:
    virtual void SetUp();
};
