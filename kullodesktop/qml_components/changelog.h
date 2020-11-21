/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <QObject>
#include <QString>
#include <QBuffer>
#include <desktoputil/asynchttpgetmanager.h>

namespace KulloDesktop {
namespace QmlComponents {

class Changelog : public QObject
{
    Q_OBJECT

public:
    explicit Changelog(QObject *parent = nullptr);

    Q_PROPERTY(QString html READ html NOTIFY htmlChanged)
    QString html() const;

    Q_INVOKABLE void update();

signals:
    void htmlChanged();

private slots:
    void onRequestFinished(DesktopUtil::AsyncHttpGetManager::Response response);

private:
    DesktopUtil::AsyncHttpGetManager manager_;
    std::shared_ptr<QByteArray> downloadData_;
    std::shared_ptr<QBuffer> downloadBuffer_;
    QString changelogHtml_;
};

}
}
