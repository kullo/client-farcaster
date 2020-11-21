/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <unordered_set>
#include <queue>
#include <QObject>
#include <QUrl>

#include <kulloclient/api/Address_base.h>
#include <kulloclient/api/DateTime_base.h>
#include <kulloclient/api/Session.h>

#include "kullodesktop/util/attachmentexporter.h"

class QJsonObject;

namespace KulloDesktop {
namespace Util {

class DataExporter : public QObject
{
    Q_OBJECT

public:
    DataExporter(std::shared_ptr<Kullo::Api::Session> session, const QUrl &destination);
    void exportDataAsync();

signals:
    void didFinish();
    void didError(QString error);

private:
    using IdTimestampTuple = std::tuple<int64_t, Kullo::Api::DateTime>;
    using IdPathFilenameTuple = std::tuple<int64_t, QString, QString>;

    void writeUserSettingsJson(QJsonObject &json);
    void writeMessageJson(QJsonObject &messageJson, int64_t msgId);
    void writeConversationsJson(QJsonObject &json);
    void writeJson();
    void exportAvatarIfNecessary(const QString &avatarPath, const std::vector<uint8_t> &image);
    void writeCss();
    void exportHtml();
    void exportConversationHtml(int64_t convId, const QString &filename);
    std::vector<IdTimestampTuple> allConversations();
    std::vector<IdTimestampTuple> allMessages(int64_t convId);
    std::vector<IdPathFilenameTuple> allAttachments(int64_t msgId);
    QString formatParticipants(std::unordered_set<Kullo::Api::Address> addresses);
    void exportAttachments();
    void exportNextAttachment();

    std::shared_ptr<Kullo::Api::Session> session_;
    QString destinationPath_;
    QString avatarsPath_;
    QString attachmentsPath_;

    struct AttachmentWorkItem
    {
        int64_t msgId;
        int64_t attId;
        std::string path;

        AttachmentWorkItem(int64_t msgId, int64_t attId, const std::string &path);
    };

    std::queue<AttachmentWorkItem> attachmentsToSave_;
    std::shared_ptr<AttachmentExporter> attachmentExporter_;
    std::shared_ptr<Kullo::Api::AsyncTask> attachmentSaveTask_;

private slots:
    void onExportingAttachmentDidFinish(int64_t msgId, int64_t attId, const std::string &path);
    void onExportingAttachmentDidError(int64_t msgId, int64_t attId, const std::string &path, Kullo::Api::LocalError error);
};

}
}
