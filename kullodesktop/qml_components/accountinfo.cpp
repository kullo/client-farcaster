/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "accountinfo.h"

#include <string>
#include <QUrl>
#include <QDesktopServices>
#include <QQmlEngine>

#include <apimirror/SessionAccountInfoListener.h>
#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api/AsyncTask.h>
#include <kulloclient/api_impl/debug.h>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/usersettings.h"

namespace KulloDesktop {
namespace QmlComponents {

AccountInfo::AccountInfo(QObject *parent)
    : QObject(parent)
    , locked_(false)
{
}

AccountInfo::~AccountInfo()
{
}

Qml::Inbox *AccountInfo::inbox()
{
    QQmlEngine::setObjectOwnership(inbox_, QQmlEngine::CppOwnership);
    return inbox_;
}

void AccountInfo::setInbox(Qml::Inbox *inbox)
{
    if (inbox_ == inbox) return;

    inbox_ = inbox;
    emit inboxChanged();
}

QString AccountInfo::planName() const
{
    if (accountInfo_ && accountInfo_->planName)
    {
        return QString::fromStdString(*accountInfo_->planName);
    }
    else
    {
        return "";
    }
}

qreal AccountInfo::storageQuota() const
{
    if (accountInfo_ && accountInfo_->storageQuota)
    {
        return *accountInfo_->storageQuota;
    }
    else
    {
        // default to an empty bar (0 of 1)
        return 1;
    }
}

qreal AccountInfo::storageUsed() const
{
    if (accountInfo_ && accountInfo_->storageUsed)
    {
        return *accountInfo_->storageUsed;
    }
    else
    {
        // default to an empty bar (0 of 1)
        return 0;
    }
}

void AccountInfo::openSettingsLocationUrl()
{
    kulloAssert(inbox_);

    auto session = inbox_->session();
    if (!session) return;

    if (locked_) return;
    locked_ = true;

    auto listener = Kullo::nn_make_shared<ApiMirror::SessionAccountInfoListener>();

    // success
    connect(listener.get(), &ApiMirror::SessionAccountInfoListener::_finished,
            this, [this](const ApiMirror::SignalSlotValue<Kullo::Api::AccountInfo> &accountInfo)
    {
        const auto urlString = QString::fromStdString(accountInfo->settingsUrl.get_value_or(""));
        const auto url = QUrl(urlString);

        if (url.isValid())
        {
            if (!QDesktopServices::openUrl(url))
            {
                Log.e() << "Could not open URL: " << url;
            }
        }
        else
        {
            Log.e() << "Invalid URL received: " << urlString;
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

    task_ = session->accountInfoAsync(listener).as_nullable();
}

void AccountInfo::reload()
{
    kulloAssert(inbox_);

    auto session = inbox_->session();
    if (!session) return;

    if (locked_) return;
    locked_ = true;

    auto listener = Kullo::nn_make_shared<ApiMirror::SessionAccountInfoListener>();

    // success
    connect(listener.get(), &ApiMirror::SessionAccountInfoListener::_finished,
            this, &AccountInfo::onNewAccountInfoReceived);

    connect(listener.get(), &ApiMirror::SessionAccountInfoListener::_error,
            this, [this](Kullo::Api::NetworkError error)
    {
        Log.e() << "Error while retrieving account info: " << error;

        this->locked_ = false;
    });

    task_ = session->accountInfoAsync(listener).as_nullable();
}

void AccountInfo::onNewAccountInfoReceived(const ApiMirror::SignalSlotValue<Kullo::Api::AccountInfo> &accountInfo)
{
    this->locked_ = false;

    auto old = accountInfo_;

    accountInfo_ = *accountInfo; // set before emitting changed signals

    if (!old || old->storageQuota != accountInfo->storageQuota)
    {
        emit storageQuotaChanged();
    }

    if (!old || old->storageUsed != accountInfo->storageUsed)
    {
        emit storageUsedChanged();
    }
}

}
}
