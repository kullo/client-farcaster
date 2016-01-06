/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <string>
#include <QObject>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/protocol/accountclient.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/util/asynctask.h>

namespace Kullo {
namespace Model {

class AccountInfo : public QObject
{
    Q_OBJECT

public:
    explicit AccountInfo(
            const Util::KulloAddress &address,
            const Util::MasterKey &masterKey,
            QObject *parent = 0);
    void update();

signals:
    void settingsUrlChanged(const std::string &settingsUrl);
    void error(std::exception_ptr exception);

private:
    Protocol::AccountClient accountClient_;

    // should be the last member (see AsyncTask docs)
    Util::AsyncTask task_;

    K_DISABLE_COPY(AccountInfo);
};

}
}
