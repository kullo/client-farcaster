/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QSortFilterProxyModel>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class ConversationListModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ConversationListModel(QObject *parent = 0);
    explicit ConversationListModel(std::shared_ptr<ConversationListSource> conversations, QObject *parent = 0);
    ~ConversationListModel();

    Q_PROPERTY(bool todoMode READ todoMode WRITE setTodoMode NOTIFY todoModeChanged)
    bool todoMode() const;
    void setTodoMode(bool active);

    Q_INVOKABLE KulloDesktop::Qml::ConversationModel *get(quint32 conversationId) const;
    Q_INVOKABLE int count() const;
    Q_INVOKABLE QVariantMap at(int row);

    Q_INVOKABLE quint32 openWhenCreated();

signals:
    void conversationsChanged();
    void todoModeChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    bool todoMode_ = false;
    std::shared_ptr<ConversationListSource> source_;
};

}
}
