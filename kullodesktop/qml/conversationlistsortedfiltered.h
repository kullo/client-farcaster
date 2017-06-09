/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QSortFilterProxyModel>

#include <kulloclient/types.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class ConversationListSortedFiltered : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ConversationListSortedFiltered(QObject *parent = 0);
    explicit ConversationListSortedFiltered(ConversationListSource *conversations, QObject *parent = 0);
    ~ConversationListSortedFiltered();

    Q_PROPERTY(bool todoMode READ todoMode WRITE setTodoMode NOTIFY todoModeChanged)
    bool todoMode() const;
    void setTodoMode(bool active);

    Q_INVOKABLE KulloDesktop::Qml::ConversationModel *get(Kullo::id_type conversationId) const;
    Q_INVOKABLE int count() const;
    Q_INVOKABLE int find(Kullo::id_type conversationId) const;
    Q_INVOKABLE QVariantMap at(int row);
    /// Searches conversation containing ch down (direction = 1) or up (direction = -1)
    /// starting from `border` (exclusive)
    Q_INVOKABLE int searchChar(const QString &ch, const int direction, const int border);

    Q_INVOKABLE Kullo::id_type openWhenCreated();

signals:
    void conversationsChanged();
    void todoModeChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    bool searchCharInConversation(const QChar character, const int row);

    bool todoMode_ = false;
    ConversationListSource *source_;
};

}
}
