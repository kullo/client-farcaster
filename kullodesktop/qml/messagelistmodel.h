/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QSortFilterProxyModel>

#include <apimirror/eventdispatcher.h>
#include <kulloclient/types.h>
#include <kulloclient/api/Session.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class MessageListModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MessageListModel(QObject *parent = nullptr);
    explicit MessageListModel(
            const std::shared_ptr<Kullo::Api::Session> &session,
            ApiMirror::EventDispatcher &eventDispatcher,
            Kullo::id_type convId,
            QObject *parent = nullptr);
    ~MessageListModel();

    Q_PROPERTY(bool todoMode READ todoMode WRITE setTodoMode NOTIFY todoModeChanged)
    bool todoMode() const;
    void setTodoMode(bool active);

    Q_INVOKABLE void markAllMessagesAsRead();
    Q_INVOKABLE void markAllMessagesAsDone();
    Q_INVOKABLE void markAllMessagesAsReadAndDone();

    Q_INVOKABLE KulloDesktop::Qml::MessageModel *get(Kullo::id_type messageId) const;
    Q_INVOKABLE void deleteMessage(Kullo::id_type messageId);

signals:
    void todoModeChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    bool todoMode_ = false;
    std::unique_ptr<MessageListSource> source_;
};

}
}
