/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <QObject>

#include <kulloclient/api/LocalError.h>
#include <kulloclient/api/MessageAttachmentsSaveToListener.h>

namespace KulloDesktop {
namespace Util {

class AttachmentExporter : public QObject, public Kullo::Api::MessageAttachmentsSaveToListener
{
    Q_OBJECT

public:
    void finished(int64_t msgId, int64_t attId, const std::string &path) override;
    void error(int64_t msgId, int64_t attId, const std::string &path, Kullo::Api::LocalError error) override;

signals:
    void didFinish(int64_t msgId, int64_t attId, const std::string &path);
    void didError(int64_t msgId, int64_t attId, const std::string &path, Kullo::Api::LocalError error);
};

}
}
