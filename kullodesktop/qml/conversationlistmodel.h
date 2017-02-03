/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QSortFilterProxyModel>

#include <kulloclient/types.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class ConversationListModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ConversationListModel(QObject *parent = 0);
    explicit ConversationListModel(ConversationListSource *conversations, QObject *parent = 0);
    ~ConversationListModel();

    Q_PROPERTY(bool todoMode READ todoMode WRITE setTodoMode NOTIFY todoModeChanged)
    bool todoMode() const;
    void setTodoMode(bool active);

    Q_INVOKABLE KulloDesktop::Qml::ConversationModel *get(Kullo::id_type conversationId) const;
    Q_INVOKABLE int count() const;
    Q_INVOKABLE int find(Kullo::id_type conversationId) const;
    Q_INVOKABLE QVariantMap at(int row);

    Q_INVOKABLE Kullo::id_type openWhenCreated();

signals:
    void conversationsChanged();
    void todoModeChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    bool todoMode_ = false;
    ConversationListSource *source_;
};

}
}
