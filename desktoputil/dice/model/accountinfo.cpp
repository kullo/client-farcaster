/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "accountinfo.h"

#include <kulloclient/util/usersettings.h>

namespace Kullo {
namespace Model {

AccountInfo::AccountInfo(
        const Util::KulloAddress &address,
        const Util::MasterKey &masterKey,
        QObject *parent)
    : QObject(parent)
    , accountClient_(address, masterKey)
{
}

void AccountInfo::update()
{
    task_.start(
        // worker
        [this]()
        {
            try
            {
                auto settingsUrl = accountClient_.getSettingsLocation();
                if (!settingsUrl.empty()) emit settingsUrlChanged(settingsUrl);
            }
            catch (...)
            {
                emit error(std::current_exception());
            }
        },

        // onCancel
        [this]()
        {
            accountClient_.cancel();
        }
    );
}

}
}
