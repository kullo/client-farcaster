/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "settingslocationmodel.h"

#include <string>
#include <QUrl>
#include <QDesktopServices>

#include <kulloclient/protocol/exceptions.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/usersettings.h>

#include <desktoputil/qtypestreamers.h>
#include <desktoputil/dice/model/accountinfo.h>

#include "kullodesktop/qml/usersettingsmodel.h"

namespace KulloDesktop {
namespace Qml {

SettingsLocationModel::SettingsLocationModel(QObject *parent)
    : QObject(parent)
    , locked_(false)
{
}

SettingsLocationModel::~SettingsLocationModel()
{
}

UserSettingsModel *SettingsLocationModel::userSettings() const
{
    return userSettings_;
}

void SettingsLocationModel::setUserSettings(UserSettingsModel *settings)
{
    if (userSettings_ != settings)
    {
        userSettings_ = settings;
        emit userSettingsChanged();
    }
}

void SettingsLocationModel::openUrl()
{
    if (locked_) return;
    locked_ = true;

    accountInfo_ = Kullo::make_unique<Kullo::Model::AccountInfo>(
                *userSettings_->rawUserSettings()->address,
                *userSettings_->rawUserSettings()->masterKey);

    // success
    connect(accountInfo_.get(), &Kullo::Model::AccountInfo::settingsUrlChanged,
            [this](const std::string &settingsUrl)
    {
        auto url = QUrl(QString::fromStdString(settingsUrl));
        if (!url.isValid())
        {
            Log.e() << "Invalid URL received: " << settingsUrl;
        }

        if (!QDesktopServices::openUrl(url))
        {
            Log.e() << "Could not open URL: " << url;
        }

        this->locked_ = false;
    });
    // error
    connect(accountInfo_.get(), &Kullo::Model::AccountInfo::error,
            [this](std::exception_ptr exception)
    {
        try {
            std::rethrow_exception(exception);
        }
        catch (Kullo::Protocol::InternalServerError &ex)
        {
            Log.e() << "HTTP 500 Internal Server Error: "
                    << Kullo::Util::formatException(ex);
        }
        catch (Kullo::Protocol::ProtocolError &ex)
        {
            Log.e() << "ProtocolError: "
                    << Kullo::Util::formatException(ex);
        }
        catch (Kullo::Protocol::NetworkError &ex)
        {
            Log.e() << "NetworkError: "
                    << Kullo::Util::formatException(ex);
        }

        this->locked_ = false;
    });

    accountInfo_->update();
}

}
}
