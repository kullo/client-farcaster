/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "conversationlistsorted.h"

#include <QDateTime>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/conversationlistsource.h"

namespace KulloDesktop {
namespace Qml {

namespace {
const auto SORT_ROLE = ConversationListSource::LatestMessageTimestampRole;
}

ConversationListSorted::ConversationListSorted(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    Log.f() << "Don't instantiate in QML.";
}

ConversationListSorted::ConversationListSorted(ConversationListSource *conversations, QObject *parent)
    : QSortFilterProxyModel(parent)
    , source_(conversations)
{
    // Do not sort automatically. Sort when source emits conversationsChanged
    setDynamicSortFilter(false);

    setSourceModel(source_);
    sort(0, Qt::DescendingOrder);

    connect(source_, &ConversationListSource::conversationsChanged, this, [&]() {
        sort(0, Qt::DescendingOrder);
        emit conversationsChanged();
    });
}

ConversationListSorted::~ConversationListSorted()
{
}

ConversationModel *ConversationListSorted::get(Kullo::id_type conversationId) const
{
    return source_->get(conversationId);
}

int ConversationListSorted::find(Kullo::id_type conversationId) const
{
    for (int row = 0; row < rowCount(); ++row)
    {
        QModelIndex idx = index(row, 0);
        if (idx.data(ConversationListSource::IdRole).toUInt() == conversationId)
        {
            return row;
        }
    }

    return -1;
}

QVariantMap ConversationListSorted::at(int row)
{
    QHash<int, QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> rolesIterator(roles);
    QVariantMap out;
    while (rolesIterator.hasNext())
    {
        rolesIterator.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(rolesIterator.key());
        out[rolesIterator.value()] = data;
    }
    return out;
}

bool ConversationListSorted::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData  = sourceModel()->data(left, SORT_ROLE);
    QVariant rightData = sourceModel()->data(right, SORT_ROLE);
    auto leftValue  = leftData.toDateTime();
    auto rightValue = rightData.toDateTime();
    return leftValue < rightValue;
}

}
}
