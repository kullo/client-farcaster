/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "settingslocationmodel.h"

#include <string>
#include <QUrl>
#include <QDesktopServices>

#include <apimirror/SessionAccountInfoListener.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api_impl/debug.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

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

void SettingsLocationModel::openUrl(ClientModel *clientModel)
{
    kulloAssert(clientModel);

    auto session = clientModel->session();
    if (!session) return;

    if (locked_) return;
    locked_ = true;

    auto listener = std::make_shared<ApiMirror::SessionAccountInfoListener>();

    // success
    connect(listener.get(), &ApiMirror::SessionAccountInfoListener::_finished,
            this, [this](const std::string &settingsUrl)
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
    connect(listener.get(), &ApiMirror::SessionAccountInfoListener::_error,
            this, [this](Kullo::Api::NetworkError error)
    {
        Log.e() << "Error while retrieving account info: " << error;

        this->locked_ = false;
    });

    task_ = session->accountInfoAsync(listener);
}

}
}
