/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "attachmentexporter.h"

namespace KulloDesktop {
namespace Util {

void AttachmentExporter::finished(int64_t msgId, int64_t attId, const std::string &path) {
    emit didFinish(msgId, attId, path);
}

void AttachmentExporter::error(int64_t msgId, int64_t attId, const std::string &path, Kullo::Api::LocalError error) {
    emit didError(msgId, attId, path, error);
}

}
}
