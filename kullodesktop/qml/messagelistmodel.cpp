/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "messagelistmodel.h"

#include <algorithm>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/messagelistsource.h"
#include "kullodesktop/qml/messagemodel.h"

namespace KulloDesktop {
namespace Qml {

namespace {
const auto SORT_ROLE = MessageListSource::MessageDateReceivedRole;
}

MessageListModel::MessageListModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    Log.e() << "Don't instantiate MessageList in QML.";
}

MessageListModel::MessageListModel(std::shared_ptr<Kullo::Model::Conversation> conv, QObject *parent)
    : QSortFilterProxyModel(parent)
    , source_(new MessageListSource(conv, nullptr))
{
    setSourceModel(source_.get());
    sort(0, Qt::DescendingOrder);
    setDynamicSortFilter(true);
}

MessageListModel::~MessageListModel()
{
}

bool MessageListModel::todoMode() const
{
    return todoMode_;
}

void MessageListModel::setTodoMode(bool active)
{
    if (todoMode_ == active) return;

    todoMode_ = active;
    invalidateFilter();
    emit todoModeChanged();
}

void MessageListModel::markAllMessagesAsRead()
{
    source_->markAllMessagesAsRead();
}

void MessageListModel::markAllMessagesAsDone()
{
    source_->markAllMessagesAsDone();
}

void MessageListModel::markAllMessagesAsReadAndDone()
{
    source_->markAllMessagesAsReadAndDone();
}

MessageModel *MessageListModel::get(quint32 messageId) const
{
    return source_->get(messageId);
}

void MessageListModel::deleteMessage(quint32 messageId)
{
    source_->deleteMessage(messageId);
}

bool MessageListModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (todoMode_)
    {
        QModelIndex sourceIndex = source_->index(sourceRow, 0, sourceParent);
        bool done = source_->data(sourceIndex, MessageListSource::MessageDoneRole).toBool();
        if (done)
        {
            return false;
        }
    }

    return true;
}

bool MessageListModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData  = sourceModel()->data(left, SORT_ROLE);
    QVariant rightData = sourceModel()->data(right, SORT_ROLE);
    auto leftValue  = leftData.toDateTime();
    auto rightValue = rightData.toDateTime();
    return leftValue < rightValue;
}

}
}
