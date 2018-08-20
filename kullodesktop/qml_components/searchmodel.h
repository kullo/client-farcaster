/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#pragma once

#include <memory>
#include <QAbstractListModel>

#include <kulloclient/kulloclient-forwards.h>
#include <kulloclient/nn.h>
#include <apimirror/apimirror-forwards.h>

#include "kullodesktop/farcaster-forwards.h"

namespace KulloDesktop {
namespace QmlComponents {

class SearchModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum SearchResultRoles {
        MessageIdRole = Qt::UserRole, // 256
        ConversationIdRole,
        HasAttachmentsRole,
        SnippetRole,
        SenderDisplayNameRole,
        DateRole,
    };

    explicit SearchModel(QObject *parent = 0);
    ~SearchModel();

    Q_PROPERTY(KulloDesktop::Qml::Inbox* inbox READ inbox WRITE setInbox NOTIFY inboxChanged)
    Qml::Inbox *inbox();
    void setInbox(Qml::Inbox *inbox);

    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    QString query() const;
    void setQuery(QString query);

    Q_PROPERTY(qlonglong conversationId READ conversationId WRITE setConversationId NOTIFY conversationIdChanged)
    qlonglong conversationId() const;
    void setConversationId(qlonglong conversationId);

    Q_PROPERTY(QString direction READ direction WRITE setDirection NOTIFY directionChanged)
    QString direction() const;
    void setDirection(QString direction);

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE void updateResults();

signals:
    void inboxChanged();
    void queryChanged();
    void conversationIdChanged();
    void directionChanged();

    // needed because ListView cannot react properly to data changes
    // https://bugreports.qt.io/browse/QTBUG-61057
    void resultsChanged();

private:
    Qml::Inbox *inbox_ = nullptr;
    QString query_;
    qlonglong conversationId_ = -1;
    QString direction_;
    std::vector<Kullo::Api::MessagesSearchResult> results_;
    Kullo::nn_shared_ptr<ApiMirror::MessagesSearchListener> searchListener_;
    std::shared_ptr<Kullo::Api::AsyncTask> searchTask_;
};

}
}
