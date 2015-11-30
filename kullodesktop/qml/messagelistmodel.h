/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#pragma once

#include <QSortFilterProxyModel>

#include <desktoputil/dice/dice-forwards.h>
#include <desktoputil/dice/model/conversation.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class MessageListModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MessageListModel(QObject *parent = 0);
    explicit MessageListModel(std::shared_ptr<Kullo::Model::Conversation> conv, QObject *parent);
    ~MessageListModel();

    Q_PROPERTY(bool todoMode READ todoMode WRITE setTodoMode NOTIFY todoModeChanged)
    bool todoMode() const;
    void setTodoMode(bool active);

    Q_INVOKABLE void markAllMessagesAsRead();
    Q_INVOKABLE void markAllMessagesAsDone();
    Q_INVOKABLE void markAllMessagesAsReadAndDone();

    Q_INVOKABLE KulloDesktop::Qml::MessageModel *get(quint32 messageId) const;
    Q_INVOKABLE void deleteMessage(quint32 messageId);

signals:
    void todoModeChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    bool todoMode_ = false;
    std::unique_ptr<MessageListSource> source_;
};

}
}
