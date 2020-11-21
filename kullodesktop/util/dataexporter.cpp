/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "dataexporter.h"

#include <algorithm>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QTextStream>
#include <QTimeZone>

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/api_impl/Address.h>
#include <kulloclient/api_impl/DateTime.h>
#include <kulloclient/api/Conversations.h>
#include <kulloclient/api/MessageAttachments.h>
#include <kulloclient/api/Messages.h>
#include <kulloclient/api/Senders.h>
#include <kulloclient/api/UserSettings.h>
#include <kulloclient/util/librarylogger.h>

static const QString JSON_NAME = "data.json";
static const QString AVATARS_DIR = "avatars";
static const QString ATTACHMENTS_DIR = "attachments";

namespace KulloDesktop {
namespace Util {

QString base64Encode(const std::vector<uint8_t> &bytes)
{
    auto byteArray = QByteArray::fromRawData(reinterpret_cast<const char *>(bytes.data()), bytes.size());
    return QString::fromStdString(byteArray.toBase64().toStdString());
}

QString hash(const std::vector<uint8_t> &bytes)
{
    QCryptographicHash sha1(QCryptographicHash::Sha1);
    sha1.addData(reinterpret_cast<const char *>(bytes.data()), bytes.size());
    return QString::fromStdString(sha1.result().toHex().toStdString());
}

QString imageFilename(const std::string &mimeType, const std::vector<uint8_t> &image)
{
    auto imageHash = hash(image);

    QString extension;
    if (mimeType == "image/png") {
        extension = "png";
    } else if (mimeType == "image/jpeg") {
        extension = "jpeg";
    } else {
        extension = "bin";
    }

    return imageHash + "." + extension;
}

QString fullNameHtml(const std::string &name, const std::string &organization)
{
    return QString::fromStdString(name).toHtmlEscaped()
        + (!organization.empty() ? " (" + QString::fromStdString(organization).toHtmlEscaped() + ")" : "");
}

DataExporter::DataExporter(std::shared_ptr<Kullo::Api::Session> session, const QUrl &destination)
    : session_(session)
    , destinationPath_(destination.toLocalFile())
    , avatarsPath_(destinationPath_ + "/" + AVATARS_DIR)
    , attachmentsPath_(destinationPath_ + "/" + ATTACHMENTS_DIR)
{}

void DataExporter::exportDataAsync()
{
    Log.i() << "Exporting data to " << destinationPath_;

    try {
        if (QFileInfo(destinationPath_).exists())
        {
            throw std::runtime_error("File or folder already exists: " + destinationPath_.toStdString());
        }

        if (!QDir().mkpath(destinationPath_))
        {
            throw std::runtime_error("Couldn't create destination folder: " + destinationPath_.toStdString());
        }

        writeJson();

        if (!QDir().mkpath(avatarsPath_))
        {
            throw std::runtime_error("Couldn't create avatars folder: " + avatarsPath_.toStdString());
        }

        if (!QDir().mkpath(attachmentsPath_))
        {
            throw std::runtime_error("Couldn't create attachments folder: " + attachmentsPath_.toStdString());
        }

        writeCss();
        exportHtml();
        exportAttachments();
    } catch (const std::runtime_error &error) {
        emit didError(QString::fromStdString(error.what()));
        attachmentsToSave_ = {};
        attachmentExporter_.reset();
    }
}

void DataExporter::writeUserSettingsJson(QJsonObject &json)
{
    auto userSettings = session_->userSettings();
    QJsonObject userSettingsJson;
    userSettingsJson["address"] = QString::fromStdString(userSettings->address().toString());
    userSettingsJson["name"] = QString::fromStdString(userSettings->name());
    userSettingsJson["organization"] = QString::fromStdString(userSettings->organization());
    userSettingsJson["footer"] = QString::fromStdString(userSettings->footer());
    userSettingsJson["avatarMimeType"] = QString::fromStdString(userSettings->avatarMimeType());
    userSettingsJson["avatarFilename"] = imageFilename(userSettings->avatarMimeType(), userSettings->avatar());

    json["userSettings"] = userSettingsJson;
}

void DataExporter::writeMessageJson(QJsonObject &messageJson, int64_t msgId)
{
    auto messages = session_->messages();
    auto senders = session_->senders();

    QJsonObject senderJson;
    senderJson["name"] = QString::fromStdString(senders->name(msgId));
    senderJson["address"] = QString::fromStdString(senders->address(msgId)->toString());
    senderJson["organization"] = QString::fromStdString(senders->organization(msgId));
    senderJson["avatarMimeType"] = QString::fromStdString(senders->avatarMimeType(msgId));
    senderJson["avatarFilename"] = imageFilename(senders->avatarMimeType(msgId), senders->avatar(msgId));

    messageJson["id"] = static_cast<qint64>(msgId);
    messageJson["sender"] = senderJson;
    messageJson["dateSent"] = QString::fromStdString(messages->dateSent(msgId).toString());
    messageJson["dateReceived"] = QString::fromStdString(messages->dateReceived(msgId).toString());
    messageJson["text"] = QString::fromStdString(messages->text(msgId));
    messageJson["footer"] = QString::fromStdString(messages->footer(msgId));
}

void DataExporter::writeConversationsJson(QJsonObject &json)
{
    QJsonArray conversationsJson;
    for (auto convId : session_->conversations()->all()) {
        QJsonObject conversationJson;
        conversationJson["id"] = static_cast<qint64>(convId);

        QJsonArray participantsJson;
        for (const auto &address : session_->conversations()->participants(convId)) {
            participantsJson.append(QString::fromStdString(address.toString()));
        }
        conversationJson["participants"] = participantsJson;

        QJsonArray messagesJson;
        for (auto &msgId : session_->messages()->allForConversation(convId)) {
            QJsonObject messageJson;
            writeMessageJson(messageJson, msgId);
            messagesJson.append(messageJson);
        }
        conversationJson["messages"] = messagesJson;

        conversationsJson.append(conversationJson);
    }

    json["conversations"] = conversationsJson;
}

void DataExporter::writeJson()
{
    QFile file(destinationPath_ + "/" + JSON_NAME);
    if (!file.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error("Couldn't open JSON file: " + file.fileName().toStdString());
    }

    QJsonObject root;
    writeUserSettingsJson(root);
    writeConversationsJson(root);
    file.write(QJsonDocument(root).toJson());
}

void DataExporter::exportAvatarIfNecessary(const QString &avatarPath, const std::vector<uint8_t> &image)
{
    if (image.size() == 0) return;

    QFile file(destinationPath_ + "/" + avatarPath);
    if (file.exists()) return;
    if (!file.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error("Couldn't open avatar file: " + avatarPath.toStdString());
    }
    file.write(reinterpret_cast<const char *>(image.data()), image.size());
}

void DataExporter::writeCss()
{
    QFile cssFile(destinationPath_ + "/style.css");
    if (!cssFile.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error("Couldn't open style.css in " + destinationPath_.toStdString());
    }
    QTextStream css(&cssFile);
    css.setCodec("UTF-8");

    css << R"CSS(
body { font-family: sans-serif; color: black; background-color: white; }
li { font-size: 12pt; list-style-type: none; margin: 16px; }
a { text-decoration: none; }
img { width: 50px; }
.sender { font-weight: bolder; }
.timestamp { font-weight: lighter; }
.timestamp a { color: black; }
.message-body { margin: 16px; max-width: 40em; }
.message-footer { margin: 16px; font-size: 80%; max-width: 40em; }
.attachment { margin-left: 16px; }
)CSS";
}

void DataExporter::exportHtml()
{
    auto conversationsPath = destinationPath_ + "/conversations";
    if (!QDir().mkpath(conversationsPath))
    {
        throw std::runtime_error("Couldn't create conversations folder: " + conversationsPath.toStdString());
    }

    QFile indexHtmlFile(destinationPath_ + "/index.html");
    if (!indexHtmlFile.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error("Couldn't open index.html in " + destinationPath_.toStdString());
    }
    QTextStream indexHtml(&indexHtmlFile);
    indexHtml.setCodec("UTF-8");

    auto userSettings = session_->userSettings();

    auto avatar = userSettings->avatar();
    auto avatarPath = AVATARS_DIR + "/" + imageFilename(userSettings->avatarMimeType(), avatar);
    exportAvatarIfNecessary(avatarPath, avatar);

    indexHtml
            << "<html><head><meta charset=\"utf-8\"><link rel=\"stylesheet\" href=\"style.css\"></head><body>" << endl

            << "<h1>" << QString::fromStdString(userSettings->address().toString()).toHtmlEscaped() << "</h1>" << endl;

    if (avatar.size() > 0) {
        indexHtml << "<a href=\"" << avatarPath << "\"><img class=\"avatar\" src=\"" << avatarPath << "\"></a>" << endl;
    }

    indexHtml
            << "<p>" << endl
            << fullNameHtml(userSettings->name(), userSettings->organization()) << endl
            << "</p>" << endl

            << "<h1>Conversations</h1>" << endl
            << "<ul>" << endl;

    for (auto convIdAndTimestamp : allConversations())
    {
        auto convId = std::get<0>(convIdAndTimestamp);
        auto participants = formatParticipants(session_->conversations()->participants(convId)).toHtmlEscaped();
        auto totalMessages = session_->conversations()->totalMessages(convId);
        auto convHtmlFilename = "conversations/" + QString::number(convId) + ".html";
        indexHtml << "<li><a href=\"" << convHtmlFilename << "\">" << participants << "</a> (" << totalMessages << " message(s))</li>" << endl;

        exportConversationHtml(convId, convHtmlFilename);
    }

    indexHtml
            << "</ul>" << endl
            << "</body></html>" << endl;
}

QString formatTimestamp(const Kullo::Api::DateTime &dateTime) {
    auto date = QDate(dateTime.year, dateTime.month, dateTime.day);
    auto time = QTime(dateTime.hour, dateTime.minute, dateTime.second);
    auto timezone = QTimeZone(dateTime.tzOffsetMinutes * 60);
    auto qDateTime = QDateTime(date, time, timezone);
    return QLocale::system().toString(qDateTime.toLocalTime());
}

void DataExporter::exportConversationHtml(int64_t convId, const QString &filename) {
    QFile convHtmlFile(destinationPath_ + "/" + filename);
    if (!convHtmlFile.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error("Couldn't open " + filename.toStdString());
    }
    QTextStream convHtml(&convHtmlFile);
    convHtml.setCodec("UTF-8");

    convHtml
            << "<html><head><meta charset=\"utf-8\"><link rel=\"stylesheet\" href=\"../style.css\"></head><body>" << endl
            << "<h1>" << formatParticipants(session_->conversations()->participants(convId)).toHtmlEscaped() << "</h1>" << endl;

    for (auto msgIdAndTimestamp : allMessages(convId))
    {
        auto msgId = std::get<0>(msgIdAndTimestamp);
        auto avatar = session_->senders()->avatar(msgId);
        auto avatarPath = AVATARS_DIR + "/" + imageFilename(session_->senders()->avatarMimeType(msgId), avatar);
        exportAvatarIfNecessary(avatarPath, avatar);

        auto formattedTimestamp = formatTimestamp(session_->messages()->dateReceived(msgId)).toHtmlEscaped();
        auto formattedText = QString::fromStdString(session_->messages()->textAsHtml(msgId, false)).replace('\n', "<br>");
        auto formattedFooter = QString::fromStdString(session_->messages()->footer(msgId)).toHtmlEscaped().replace('\n', "<br>");

        convHtml
                << "<li id=\"msg-" << QString::number(msgId) << "\">" << endl;

        if (avatar.size() > 0) {
            convHtml << "<a href=\"../" << avatarPath << "\"><img class=\"avatar\" src=\"../" << avatarPath << "\"></a>" << endl;
        }

        convHtml
                << "<div class=\"sender\">" << fullNameHtml(session_->senders()->name(msgId), session_->senders()->organization(msgId)) << ", " << QString::fromStdString(session_->senders()->address(msgId)->toString()).toHtmlEscaped() << "</div>" << endl
                << "<div class=\"timestamp\"><a href=\"#msg-" << QString::number(msgId) << "\">" << formattedTimestamp << "</a></div>" << endl
                << "<div class=\"message-body\">" << formattedText << "</div>" << endl;

        for (auto attachment : allAttachments(msgId)) {
            auto path = std::get<1>(attachment);
            auto filename = std::get<2>(attachment);
            convHtml
                    << "<div class=\"attachment\"><a href=\"../" << path.toHtmlEscaped() << "/" << filename.toHtmlEscaped() << "\">" << filename.toHtmlEscaped() << "</a></div>";
        }

        convHtml
                << "<div class=\"message-footer\">" << formattedFooter << "</div>" << endl
                << "</li>" << endl;
    }

    convHtml
            << "</body></html>" << endl;
}

std::vector<DataExporter::IdTimestampTuple> DataExporter::allConversations()
{
    auto convIds = session_->conversations()->all();
    std::vector<IdTimestampTuple> convIdsAndTimestamps;
    convIdsAndTimestamps.reserve(convIds.size());

    std::transform(convIds.begin(), convIds.end(), std::back_inserter(convIdsAndTimestamps), [this](int64_t convId) -> DataExporter::IdTimestampTuple
    {
        return std::make_tuple(convId, session_->conversations()->latestMessageTimestamp(convId));
    });

    std::sort(convIdsAndTimestamps.begin(), convIdsAndTimestamps.end(), [](IdTimestampTuple lhs, IdTimestampTuple rhs)
    {
        // Sort descending by last message timestamp
        return std::get<1>(lhs) > std::get<1>(rhs);
    });

    return convIdsAndTimestamps;
}

std::vector<DataExporter::IdTimestampTuple> DataExporter::allMessages(int64_t convId)
{
    auto msgIds = session_->messages()->allForConversation(convId);
    std::vector<IdTimestampTuple> msgIdsAndTimestamps;
    msgIdsAndTimestamps.reserve(msgIds.size());

    std::transform(msgIds.begin(), msgIds.end(), std::back_inserter(msgIdsAndTimestamps), [this](int64_t msgId) -> DataExporter::IdTimestampTuple
    {
        return std::make_tuple(msgId, session_->messages()->dateReceived(msgId));
    });

    std::sort(msgIdsAndTimestamps.begin(), msgIdsAndTimestamps.end(), [](IdTimestampTuple lhs, IdTimestampTuple rhs)
    {
        // Sort descending by timestamp
        return std::get<1>(lhs) > std::get<1>(rhs);
    });

    return msgIdsAndTimestamps;
}

std::vector<DataExporter::IdPathFilenameTuple> DataExporter::allAttachments(int64_t msgId)
{
    std::vector<IdPathFilenameTuple> attIdsAndPathsAndFilenames;
    auto dirPath = ATTACHMENTS_DIR + "/" + QString::number(msgId);
    for (auto attId : session_->messageAttachments()->allForMessage(msgId))
    {
        attIdsAndPathsAndFilenames.emplace_back(attId, dirPath, QString::fromStdString(session_->messageAttachments()->filename(msgId, attId)));
    }
    return attIdsAndPathsAndFilenames;
}

QString DataExporter::formatParticipants(std::unordered_set<Kullo::Api::Address> addresses)
{
    QStringList participants;
    participants.reserve(addresses.size());

    for (const auto &address : addresses) {
        auto senderMsgId = session_->messages()->latestForSender(address);
        participants.append(QString::fromStdString((senderMsgId >= 0) ? session_->senders()->name(senderMsgId) : address.toString()));
    }

    return participants.join(", ");
}

void DataExporter::exportAttachments()
{
    for (auto convId : session_->conversations()->all())
    {
        for (auto msgId : session_->messages()->allForConversation(convId))
        {
            for (auto attachment : allAttachments(msgId))
            {
                auto attId = std::get<0>(attachment);
                auto dirPath = destinationPath_ + "/" + std::get<1>(attachment);
                auto filename = std::get<2>(attachment);
                if (!QDir().mkpath(dirPath))
                {
                    throw std::runtime_error("Creating message attachments folder failed: " + dirPath.toStdString());
                }
                auto filePath = dirPath.toStdString() + "/" + filename.toStdString();
                attachmentsToSave_.push(AttachmentWorkItem(msgId, attId, filePath));
            }
        }
    }

    attachmentExporter_ = std::make_shared<AttachmentExporter>();
    connect(attachmentExporter_.get(), &AttachmentExporter::didFinish,
            this, &DataExporter::onExportingAttachmentDidFinish);
    connect(attachmentExporter_.get(), &AttachmentExporter::didError,
            this, &DataExporter::onExportingAttachmentDidError);
    exportNextAttachment();
}

void DataExporter::exportNextAttachment()
{
    if (attachmentsToSave_.empty()) {
        Log.i() << "Done exporting attachments.";
        attachmentExporter_.reset();
        emit didFinish();
    } else {
        auto workItem = attachmentsToSave_.front();
        attachmentsToSave_.pop();
        Log.i() << "Exporting " << workItem.path << "…";
        attachmentSaveTask_ = session_->messageAttachments()->saveToAsync(workItem.msgId, workItem.attId, workItem.path, NN_CHECK_THROW(attachmentExporter_)).as_nullable();
    }
}

void DataExporter::onExportingAttachmentDidFinish(int64_t msgId, int64_t attId, const std::string &path)
{
    K_UNUSED(msgId);
    K_UNUSED(attId);

    Log.d() << "Exported " << path;
    exportNextAttachment();
}

void DataExporter::onExportingAttachmentDidError(int64_t msgId, int64_t attId, const std::string &path, Kullo::Api::LocalError error)
{
    K_UNUSED(msgId);
    K_UNUSED(attId);

    std::string errorString;
    switch (error) {
    case Kullo::Api::LocalError::Filesystem:
        errorString = "Filesystem error";
        break;
    case Kullo::Api::LocalError::FileTooBig:
        errorString = "File too big";
        break;
    case Kullo::Api::LocalError::Unknown:
        errorString = "Unknown error";
        break;
    }
    Log.e() << "Failed to export attachment" << path << ", error:" << errorString;
    emit didError("Failed to export attachments.");
}

DataExporter::AttachmentWorkItem::AttachmentWorkItem(int64_t msgId, int64_t attId, const std::string &path)
    : msgId(msgId)
    , attId(attId)
    , path(path)
{}

}
}
