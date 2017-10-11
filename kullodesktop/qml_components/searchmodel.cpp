/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "kullodesktop/qml_components/searchmodel.h"

#include <QQmlEngine>
#include <QRegularExpression>

#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/api/MessageAttachments.h>
#include <kulloclient/api/Messages.h>
#include <kulloclient/api/MessagesSearchResult.h>
#include <kulloclient/api/SenderPredicate.h>
#include <kulloclient/api/Senders.h>
#include <kulloclient/api/Session.h>
#include <kulloclient/api/UserSettings.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/util/strings.h>

#include <apimirror/MessagesSearchListener.h>
#include <desktoputil/kulloclient2qt.h>

#include "kullodesktop/qml/inbox.h"

namespace KulloDesktop {
namespace QmlComponents {

namespace {

QString highlightHtml(const QString &snippetRaw, const QString &boundary)
{
    // cache this matcher for all results of one search?
    QRegularExpression matcher(QStringLiteral("\\(%1\\)(.*?)\\(/%1\\)").arg(boundary));
    QString out = snippetRaw;
    out.replace(matcher, QStringLiteral("<b>\\1</b>"));
    return out;
}
}

SearchModel::SearchModel(QObject *parent)
    : QAbstractListModel(parent)
    , searchListener_(Kullo::nn_make_shared<ApiMirror::MessagesSearchListener>())
{
    connect(searchListener_.get(), &ApiMirror::MessagesSearchListener::_finished, this, [&](const std::vector<Kullo::Api::MessagesSearchResult> & results) {
        beginResetModel();
        results_ = std::move(results);
        endResetModel();

        emit resultsChanged();
    });
}

SearchModel::~SearchModel()
{
}

Qml::Inbox *SearchModel::inbox()
{
    QQmlEngine::setObjectOwnership(inbox_, QQmlEngine::CppOwnership);
    return inbox_;
}

void SearchModel::setInbox(Qml::Inbox *inbox)
{
    if (inbox_ == inbox) return;

    inbox_ = inbox;
    emit inboxChanged();
}

QString SearchModel::query() const
{
    return query_;
}

void SearchModel::setQuery(QString query)
{
    if (query_ == query) return;

    query_ = query;
    emit queryChanged();
}

qlonglong SearchModel::conversationId() const
{
    return conversationId_;
}

void SearchModel::setConversationId(qlonglong conversationId)
{
    if (conversationId_ == conversationId) return;

    conversationId_ = conversationId;
    emit conversationIdChanged();
}

QString SearchModel::direction() const
{
    return direction_;
}

void SearchModel::setDirection(QString direction)
{
    if (direction_ == direction) return;

    direction_ = direction;
    emit directionChanged();
}

QHash<int, QByteArray> SearchModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MessageIdRole]              = QByteArrayLiteral("messageId_");
    roles[ConversationIdRole]         = QByteArrayLiteral("conversationId_");
    roles[HasAttachmentsRole]         = QByteArrayLiteral("hasAttachments_");
    roles[SnippetRole]                = QByteArrayLiteral("snippet_");
    roles[SenderDisplayNameRole]      = QByteArrayLiteral("sender_");
    roles[DateRole]                   = QByteArrayLiteral("date_");
    return roles;
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    auto row = static_cast<unsigned int>(index.row());
    if (index.row() < 0 || row >= results_.size())
    {
        return QVariant();
    }

    const auto &result = results_[row];

    switch (role)
    {
    case MessageIdRole:
        return qint64(result.msgId);
    case ConversationIdRole:
        return qint64(result.convId);
    case HasAttachmentsRole: {
        auto msgId = result.msgId;
        auto attachments = inbox_->session()->messageAttachments()->allForMessage(msgId);
        return !attachments.empty();
    }
    case SnippetRole:
        return highlightHtml(
                    QString::fromStdString(Kullo::Util::Strings::htmlEscape(result.snippet)),
                    QString::fromStdString(result.boundary)
                    );
    case SenderDisplayNameRole: {
        auto msgId = result.msgId;
        auto name = inbox_->session()->senders()->name(msgId);
        auto organization = inbox_->session()->senders()->organization(msgId);
        if (organization.empty()) {
            return QString::fromStdString(name);
        } else {
            return QString::fromStdString(name + " (" + organization + ")");
        }
    }
    case DateRole:
        return DesktopUtil::KulloClient2Qt::toQDateTime(
                    inbox_->session()->messages()->dateReceived(result.msgId));
    default:
        Log.e() << "SearchModel::data(): Unknown role: " << role;
        return QVariant();
    }
}

int SearchModel::rowCount(const QModelIndex &parent) const
{
    K_UNUSED(parent);
    return results_.size();
}

void SearchModel::updateResults()
{
    boost::optional<Kullo::Api::SenderPredicate> senderPredicate;
    if (direction_ == "incoming")
    {
        senderPredicate = Kullo::Api::SenderPredicate(
                    Kullo::Api::SearchPredicateOperator::IsNot,
                    inbox_->session()->userSettings()->address().toString());
    }
    else if (direction_ == "outgoing")
    {
        senderPredicate = Kullo::Api::SenderPredicate(
                    Kullo::Api::SearchPredicateOperator::Is,
                    inbox_->session()->userSettings()->address().toString());
    }
    else
    {
        senderPredicate = boost::none;
    }

    searchTask_ = inbox_->session()->messages()->searchAsync(
                query_.toStdString(),
                conversationId_,
                senderPredicate,
                100,
                boost::none,
                searchListener_
                ).as_nullable();
}

}
}
