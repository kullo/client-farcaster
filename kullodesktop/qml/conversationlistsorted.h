/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QSortFilterProxyModel>

#include <kulloclient/types.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace Qml {

class ConversationListSorted : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ConversationListSorted(QObject *parent = nullptr);
    explicit ConversationListSorted(ConversationListSource *conversations, QObject *parent = nullptr);
    ~ConversationListSorted();

    Q_INVOKABLE KulloDesktop::Qml::ConversationModel *get(Kullo::id_type conversationId) const;
    Q_INVOKABLE int find(Kullo::id_type conversationId) const;
    Q_INVOKABLE QVariantMap at(int row);

signals:
    void conversationsChanged();

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    ConversationListSource *source_;
};

}
}
