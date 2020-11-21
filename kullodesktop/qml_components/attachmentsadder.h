/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <atomic>
#include <deque>
#include <mutex>

#include <apimirror/enums/LocalErrorHolder.h>
#include <boost/optional.hpp>
#include <QObject>
#include <QUrl>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace QmlComponents {

struct AttachmentToBeAdded {
    QString source;
    int64_t size = -1;
};

// Used to add multiple attachments at once
class AttachmentsAdder : public QObject
{
    Q_OBJECT

public:
    explicit AttachmentsAdder(QObject *parent = nullptr);
    ~AttachmentsAdder() override;

    Q_PROPERTY(KulloDesktop::Qml::DraftModel* target READ target WRITE setTarget NOTIFY targetChanged)
    Qml::DraftModel *target() const;
    void setTarget(Qml::DraftModel *target);

    // Return false when currently locked, true otherwise
    Q_INVOKABLE bool addAttachments(const QList<QUrl> &urls);

signals:
    void targetChanged();
    void addingAttachmentsStarted();
    void addingAttachmentsError(
            ApiMirror::Enums::LocalErrorHolder::LocalError error,
            const QString &filename);
    void addingAttachmentsFinished();

    // This is not guaranteed to be called at all,
    // e.g. when attachments are too small
    void progressed(int percentages);

private slots:
    void onAddingAttachmentProgressed(int64_t bytesProcessed, int64_t bytesTotal);
    void onAddingAttachmentError(
            ApiMirror::Enums::LocalErrorHolder::LocalError error,
            const QString &filename);
    void onAddingAttachmentFinished();

private:
    void doneAddingAttachments();
    bool addNextAttachment();

    Qml::DraftModel* draftModel_ = nullptr;
    std::deque<AttachmentToBeAdded> attachmentsToBeAdded_;
    boost::optional<AttachmentToBeAdded> currentlyAddedAttachment_;
    std::mutex working_;
    std::atomic<bool> cancel_;

    int64_t sizeAllAttachments_ = 0;
    int64_t sizeSumReadyAttachments_ = 0;
};

}
}
