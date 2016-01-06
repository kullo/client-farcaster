/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "conversationlistmodel.h"

#include <QDateTime>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/conversationlistsource.h"

namespace KulloDesktop {
namespace Qml {

namespace {
const auto SORT_ROLE = ConversationListSource::LatestMessageTimestampRole;
}

ConversationListModel::ConversationListModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    Log.e() << "Don't instantiate ConversationListModel in QML.";
}

ConversationListModel::ConversationListModel(std::shared_ptr<ConversationListSource> conversations, QObject *parent)
    : QSortFilterProxyModel(parent)
    , source_(conversations)
{
    setSourceModel(source_.get());
    sort(0, Qt::DescendingOrder);
    setDynamicSortFilter(true);

    connect(source_.get(), &ConversationListSource::conversationsChanged,
            this, &ConversationListModel::conversationsChanged);
}

ConversationListModel::~ConversationListModel()
{
}

bool ConversationListModel::todoMode() const
{
    return todoMode_;
}

void ConversationListModel::setTodoMode(bool active)
{
    if (todoMode_ == active) return;

    todoMode_ = active;
    invalidateFilter();
    emit todoModeChanged();
}

ConversationModel *ConversationListModel::get(quint32 conversationId) const
{
    return source_->get(conversationId);
}

int ConversationListModel::count() const
{
    return source_->rowCount();
}

QVariantMap ConversationListModel::at(int row)
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

quint32 ConversationListModel::openWhenCreated()
{
    return source_->openWhenCreated();
}

bool ConversationListModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (todoMode_)
    {
        QModelIndex sourceIndex = source_->index(sourceRow, 0, sourceParent);
        bool countUndone = source_->data(sourceIndex, ConversationListSource::CountUndoneRole).toInt();
        if (countUndone == 0)
        {
            return false;
        }
    }

    return true;
}

bool ConversationListModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData  = sourceModel()->data(left, SORT_ROLE);
    QVariant rightData = sourceModel()->data(right, SORT_ROLE);
    auto leftValue  = leftData.toDateTime();
    auto rightValue = rightData.toDateTime();
    return leftValue < rightValue;
}

}
}
