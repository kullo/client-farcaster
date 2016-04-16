/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <atomic>
#include <mutex>
#include <thread>

#include <QFuture>
#include <QObject>
#include <QUrl>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class AttachmentsAdderModel : public QObject
{
    Q_OBJECT

public:
    explicit AttachmentsAdderModel(QObject *parent = 0);
    ~AttachmentsAdderModel() override;

    Q_PROPERTY(KulloDesktop::Qml::DraftModel* target READ target WRITE setTarget NOTIFY targetChanged)
    DraftModel *target() const;
    void setTarget(DraftModel *target);

    Q_INVOKABLE bool addAttachments(const QList<QUrl> &urls);

signals:
    void targetChanged();
    void addingAttachmentsStarted();
    void addingAttachmentsFinished();

    // interal
    void _doneAddingAttachments();

private slots:
    void onDoneAddingAttachments();

private:
    DraftModel* draftModel_ = nullptr;
    std::mutex working_;
    std::atomic<bool> cancel_;
    std::thread backgroundJob_;
};

}
}
