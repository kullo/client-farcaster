/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "conversationlistsortedfiltered.h"

#include <QDateTime>
#include <QRegularExpression>
#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>

#include "kullodesktop/qml/conversationlistsource.h"

namespace KulloDesktop {
namespace Qml {

namespace {
const auto SORT_ROLE = ConversationListSource::LatestMessageTimestampRole;

// lower case only
const QRegularExpression WORD_SEPARATOR(QStringLiteral("[^a-z]+"), QRegularExpression::OptimizeOnFirstUsageOption);
}

ConversationListSortedFiltered::ConversationListSortedFiltered(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    Log.f() << "Don't instantiate in QML.";
}

ConversationListSortedFiltered::ConversationListSortedFiltered(ConversationListSource *conversations, QObject *parent)
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

ConversationListSortedFiltered::~ConversationListSortedFiltered()
{
}

bool ConversationListSortedFiltered::todoMode() const
{
    return todoMode_;
}

void ConversationListSortedFiltered::setTodoMode(bool active)
{
    if (todoMode_ == active) return;

    todoMode_ = active;

    // When elements are added to the result (todo mode on -> off), resorting is required
    if (active) invalidateFilter();
    else invalidate();

    emit todoModeChanged();
}

ConversationModel *ConversationListSortedFiltered::get(Kullo::id_type conversationId) const
{
    return source_->get(conversationId);
}

int ConversationListSortedFiltered::count() const
{
    return source_->rowCount();
}

int ConversationListSortedFiltered::find(Kullo::id_type conversationId) const
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

QVariantMap ConversationListSortedFiltered::at(int row)
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

int ConversationListSortedFiltered::searchChar(const QString &ch, const int direction, const int border)
{
    kulloAssert(ch.size() == 1);
    kulloAssert(ch.toLower() == ch);

    for (int pos = border + direction; pos < count() && pos >= 0; pos += direction)
    {
        if (searchCharInConversation(ch.at(0), pos))
        {
            return pos;
        }
    }

    return -1;
}

Kullo::id_type ConversationListSortedFiltered::openWhenCreated()
{
    return source_->openWhenCreated();
}

bool ConversationListSortedFiltered::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
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

bool ConversationListSortedFiltered::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData  = sourceModel()->data(left, SORT_ROLE);
    QVariant rightData = sourceModel()->data(right, SORT_ROLE);
    auto leftValue  = leftData.toDateTime();
    auto rightValue = rightData.toDateTime();
    return leftValue < rightValue;
}

// char must be lower case
bool ConversationListSortedFiltered::searchCharInConversation(const QChar character, const int row)
{
    const QModelIndex idx = index(row, 0);

    // Check all participant addresses
    const QVariant addressesData = idx.data(ConversationListSource::ParticipantsAddressesRole);
    const QStringList addresses = addressesData.toStringList();
    for (const QString &address : addresses)
    {
        if (address.startsWith(character))
        {
            return true;
        }
    }

    // check participant names
    const QVariant titleData = idx.data(ConversationListSource::TitleRole);
    const QString title = titleData.toString().trimmed().toLower();
    for (const QString &titlePart : title.split(WORD_SEPARATOR, QString::SkipEmptyParts))
    {
        if (titlePart.startsWith(character))
        {
            return true;
        }
    }

    return false;
}

}
}
