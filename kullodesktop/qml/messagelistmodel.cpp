/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "messagelistmodel.h"

#include <algorithm>

#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/messagelistsource.h"
#include "kullodesktop/qml/messagemodel.h"

namespace KulloDesktop {
namespace Qml {

MessageListModel::MessageListModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    Log.f() << "Don't instantiate MessageList in QML.";
}

MessageListModel::MessageListModel(
        const std::shared_ptr<Kullo::Api::Session> &session,
        ApiMirror::EventDispatcher &eventDispatcher,
        Kullo::id_type convId,
        QObject *parent)
    : QSortFilterProxyModel(parent)
    , source_(new MessageListSource(session, eventDispatcher, convId, nullptr))
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

MessageModel *MessageListModel::get(Kullo::id_type messageId) const
{
    return source_->get(messageId);
}

int MessageListModel::find(Kullo::id_type messageId) const
{
    for (int row = 0; row < rowCount(); ++row)
    {
        QModelIndex idx = index(row, 0);
        if (idx.data(MessageListSource::MessageIdRole).toLongLong() == messageId)
        {
            return row;
        }
    }

    return -1;
}

void MessageListModel::deleteMessage(Kullo::id_type messageId)
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
    QVariant leftData  = sourceModel()->data(left, MessageListSource::MessageIdRole);
    QVariant rightData = sourceModel()->data(right, MessageListSource::MessageIdRole);
    auto leftValue  = leftData.toLongLong();
    auto rightValue = rightData.toLongLong();
    return leftValue < rightValue;
}

}
}
