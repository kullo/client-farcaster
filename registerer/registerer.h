/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <QCoreApplication>
#include <memory>
#include <desktoputil/dice/model/registration.h>
#include <kulloclient/kulloclient-forwards.h>

class Registerer : public QObject
{
    Q_OBJECT

public:
    explicit Registerer(const QCoreApplication &app, QObject *parent = 0);
    ~Registerer();
    void run(const Kullo::Util::KulloAddress &address, const Kullo::Util::MasterKey &masterKey);

    static std::string getStringContentsOrCrash(const std::string &filename, bool trimmed = true);

signals:
    void done();

private slots:
    void onKeysGenerationProgressChanged(int progress);
    void onKeysGenerationDone();
    void onSuccess();
    void onChallenge();
    void onAddressExists();
    void onAddressBlocked();
    void onError(std::exception_ptr exception);

private:
    const QCoreApplication &app_;
    std::unique_ptr<Kullo::Model::Registration> registration_;
    std::unique_ptr<Kullo::Util::KulloAddress> registrationAddress_;
};
