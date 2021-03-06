/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <vector>
#include <QObject>

#include <kulloclient/types.h>
#include <kulloclient/api/MessagesSearchListener.h>
#include <kulloclient/api/MessagesSearchResult.h>

#include "apimirror/misc.h"

namespace ApiMirror {

class MessagesSearchListener
        : public QObject
        , public Kullo::Api::MessagesSearchListener
{

    Q_OBJECT

public:
    explicit MessagesSearchListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        K_REGISTER_QT_META_TYPE(std::vector<Kullo::Api::MessagesSearchResult>);
    }

    void finished(const std::vector<Kullo::Api::MessagesSearchResult> & results) override {
        emit _finished(results);
    }

signals:
    void _finished(const std::vector<Kullo::Api::MessagesSearchResult> & results);
};

}
