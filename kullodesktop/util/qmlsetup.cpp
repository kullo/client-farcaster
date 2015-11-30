/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "qmlsetup.h"

#include <QQmlApplicationEngine>

#include <apimirror/Client.h>

#include "kullodesktop/imageproviders/attachmentpreviewprovider.h"
#include "kullodesktop/imageproviders/conversationavatarprovider.h"
#include "kullodesktop/imageproviders/messageavatarprovider.h"
#include "kullodesktop/imageproviders/participantavatarprovider.h"
#include "kullodesktop/imageproviders/senderavatarprovider.h"
#include "kullodesktop/imageproviders/usersettingsavatarprovider.h"

#include "kullodesktop/qml/attachmentlistmodel.h"
#include "kullodesktop/qml/attachmentmodel.h"
#include "kullodesktop/qml/attachmentsaddermodel.h"
#include "kullodesktop/qml/changelog.h"
#include "kullodesktop/qml/clientmodel.h"
#include "kullodesktop/qml/conversationlistmodel.h"
#include "kullodesktop/qml/conversationmodel.h"
#include "kullodesktop/qml/draftmodel.h"
#include "kullodesktop/qml/draftattachmentlistmodel.h"
#include "kullodesktop/qml/draftattachmentmodel.h"
#include "kullodesktop/qml/existencechecker.h"
#include "kullodesktop/qml/filesavedialog.h"
#include "kullodesktop/qml/fileopendialog.h"
#include "kullodesktop/qml/kulloupdatermodel.h"
#include "kullodesktop/qml/libraryloggermodel.h"
#include "kullodesktop/qml/loginchecker.h"
#include "kullodesktop/qml/messagelistmodel.h"
#include "kullodesktop/qml/messagemodel.h"
#include "kullodesktop/qml/participantmodel.h"
#include "kullodesktop/qml/registerer.h"
#include "kullodesktop/qml/scheduler.h"
#include "kullodesktop/qml/settingslocationmodel.h"
#include "kullodesktop/qml/usersettingsmodel.h"

#include "kullodesktop/visuals/advancedrectangle.h"
#include "kullodesktop/visuals/hint.h"

namespace KulloDesktop {
namespace Util {

void QmlSetup::setupTypes()
{
    qmlRegisterType<FileSaveDialog>("Kullo", 1, 0, "FileSaveDialog");
    qmlRegisterType<FileOpenDialog>("Kullo", 1, 0, "FileOpenDialog");

    qmlRegisterType<Qml::AttachmentListModel>("Kullo", 1, 0, "AttachmentList");
    qmlRegisterType<Qml::AttachmentModel>("Kullo", 1, 0, "Attachment");
    qmlRegisterType<Qml::AttachmentsAdderModel>("Kullo", 1, 0, "AttachmentsAdder");
    qmlRegisterType<Qml::ChallengeTypes>("Kullo", 1, 0, "ChallengeTypes");
    qmlRegisterType<Qml::Changelog>("Kullo", 1, 0, "Changelog");
    qmlRegisterType<Qml::ConversationListModel>("Kullo", 1, 0, "ConversationListModel");
    qmlRegisterType<Qml::ConversationModel>("Kullo", 1, 0, "Conversation");
    qmlRegisterType<Qml::DraftModel>("Kullo", 1, 0, "Draft");
    qmlRegisterType<Qml::DraftAttachmentListModel>("Kullo", 1, 0, "DraftAttachmentList");
    qmlRegisterType<Qml::DraftAttachmentModel>("Kullo", 1, 0, "DraftAttachment");
    qmlRegisterType<Qml::ExistenceChecker>("Kullo", 1, 0, "ExistenceChecker");
    qmlRegisterType<Qml::KulloUpdaterModel>("Kullo", 1, 0, "KulloUpdater");
    qmlRegisterType<Qml::LibraryLoggerModel>("Kullo", 1, 0, "LibraryLogger");
    qmlRegisterType<Qml::LoginChecker>("Kullo", 1, 0, "LoginChecker");
    qmlRegisterType<Qml::MessageListModel>("Kullo", 1, 0, "MessageList");
    qmlRegisterType<Qml::MessageModel>("Kullo", 1, 0, "Message");
    qmlRegisterType<Qml::ParticipantModel>("Kullo", 1, 0, "Participant");
    qmlRegisterType<Qml::Registerer>("Kullo", 1, 0, "Registerer");
    qmlRegisterType<Qml::Scheduler>("Kullo", 1, 0, "Scheduler");
    qmlRegisterType<Qml::SettingsLocationModel>("Kullo", 1, 0, "SettingsLocation");
    qmlRegisterType<Qml::SyncErrors>("Kullo", 1, 0, "SyncErrors");
    qmlRegisterType<Qml::UserSettingsModel>("Kullo", 1, 0, "UserSettings");

    qmlRegisterType<Visuals::AdvancedRectangle>("Kullo.Visuals", 1, 0, "AdvancedRectangle");
    qmlRegisterType<Visuals::Hint>("Kullo.Visuals", 1, 0, "Hint");

    // global object types
    qmlRegisterType<ApiMirror::Client>("Kullo", 1, 0, "ClientType");
}

void QmlSetup::setupImageproviders(QQmlApplicationEngine &engine, Qml::ClientModel &clientModel)
{
    using namespace KulloDesktop::Imageproviders;
    engine.addImageProvider(QStringLiteral("attachmentpreview"),   new AttachmentPreviewProvider(clientModel));
    engine.addImageProvider(QStringLiteral("conversationavatars"), new ConversationAvatarProvider(clientModel));
    engine.addImageProvider(QStringLiteral("messageavatars"),      new MessageAvatarProvider(clientModel));
    engine.addImageProvider(QStringLiteral("participantavatars"),  new ParticipantAvatarProvider(clientModel));
    engine.addImageProvider(QStringLiteral("senderavatars"),       new SenderAvatarProvider(clientModel));
    engine.addImageProvider(QStringLiteral("usersettingsavatars"), new UserSettingsAvatarProvider(clientModel));
}

}
}
