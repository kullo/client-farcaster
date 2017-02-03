/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
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
#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/conversationlistmodel.h"
#include "kullodesktop/qml/conversationmodel.h"
#include "kullodesktop/qml/devicesettings.h"
#include "kullodesktop/qml/draftmodel.h"
#include "kullodesktop/qml/draftattachmentlistmodel.h"
#include "kullodesktop/qml/draftattachmentmodel.h"
#include "kullodesktop/qml/innerapplication.h"
#include "kullodesktop/qml/messagelistmodel.h"
#include "kullodesktop/qml/messagemodel.h"
#include "kullodesktop/qml/sender.h"
#include "kullodesktop/qml/usersettings.h"
#include "kullodesktop/qml_components/visuals/advancedrectangle.h"
#include "kullodesktop/qml_components/visuals/hint.h"
#include "kullodesktop/qml_components/attachmentsadder.h"
#include "kullodesktop/qml_components/changelog.h"
#include "kullodesktop/qml_components/existencechecker.h"
#include "kullodesktop/qml_components/filesavedialog.h"
#include "kullodesktop/qml_components/fileopendialog.h"
#include "kullodesktop/qml_components/kulloupdatermodel.h"
#include "kullodesktop/qml_components/libraryloggerqmlconnector.h"
#include "kullodesktop/qml_components/loginchecker.h"
#include "kullodesktop/qml_components/registerer.h"
#include "kullodesktop/qml_components/scheduler.h"
#include "kullodesktop/qml_components/settingslocation.h"

namespace KulloDesktop {
namespace QmlBridge {

void QmlSetup::setupTypes()
{
    // Components to be instantiated by Qml
    qmlRegisterType<FileSaveDialog>("Kullo", 1, 0, "FileSaveDialog");
    qmlRegisterType<FileOpenDialog>("Kullo", 1, 0, "FileOpenDialog");
    qmlRegisterType<QmlComponents::AttachmentsAdder>("Kullo", 1, 0, "AttachmentsAdder");
    qmlRegisterType<QmlComponents::ChallengeTypes>("Kullo", 1, 0, "ChallengeTypes");
    qmlRegisterType<QmlComponents::Changelog>("Kullo", 1, 0, "Changelog");
    qmlRegisterType<QmlComponents::ExistenceChecker>("Kullo", 1, 0, "ExistenceChecker");
    qmlRegisterType<QmlComponents::KulloUpdaterModel>("Kullo", 1, 0, "KulloUpdater");
    qmlRegisterType<QmlComponents::LibraryLoggerQmlConnector>("Kullo", 1, 0, "LibraryLoggerQmlConnector");
    qmlRegisterType<QmlComponents::LoginChecker>("Kullo", 1, 0, "LoginChecker");
    qmlRegisterType<QmlComponents::Registerer>("Kullo", 1, 0, "Registerer");
    qmlRegisterType<QmlComponents::Scheduler>("Kullo", 1, 0, "Scheduler");
    qmlRegisterType<QmlComponents::SettingsLocation>("Kullo", 1, 0, "SettingsLocation");
    qmlRegisterType<QmlComponents::Visuals::AdvancedRectangle>("Kullo.Visuals", 1, 0, "AdvancedRectangle");
    qmlRegisterType<QmlComponents::Visuals::Hint>("Kullo.Visuals", 1, 0, "Hint");

    qmlRegisterType<Qml::AttachmentListModel>("Kullo", 1, 0, "AttachmentList");
    qmlRegisterType<Qml::AttachmentModel>("Kullo", 1, 0, "Attachment");
    qmlRegisterType<Qml::ConversationListModel>("Kullo", 1, 0, "ConversationList");
    qmlRegisterType<Qml::ConversationModel>("Kullo", 1, 0, "Conversation");
    qmlRegisterType<Qml::DeviceSettings>("Kullo", 1, 0, "DeviceSettings");
    qmlRegisterType<Qml::DraftModel>("Kullo", 1, 0, "Draft");
    qmlRegisterType<Qml::DraftAttachmentListModel>("Kullo", 1, 0, "DraftAttachmentList");
    qmlRegisterType<Qml::DraftAttachmentModel>("Kullo", 1, 0, "DraftAttachment");
    qmlRegisterType<Qml::MessageListModel>("Kullo", 1, 0, "MessageList");
    qmlRegisterType<Qml::MessageModel>("Kullo", 1, 0, "Message");
    qmlRegisterType<Qml::Sender>("Kullo", 1, 0, "Sender");
    qmlRegisterType<Qml::SyncErrors>("Kullo", 1, 0, "SyncErrors");

    qmlRegisterUncreatableType<Qml::UserSettings>("Kullo", 1, 0, "UserSettings", "Do not create in QML");

    // Registered as global objects QQmlContext::setContextProperty()
    // Use qmlRegisterType() with no arguments because "Instances of this
    // type cannot be created from the QML system."
    qmlRegisterType<ApiMirror::Client>();
    qmlRegisterType<Qml::Inbox>();
    qmlRegisterType<Qml::InnerApplication>();
}

void QmlSetup::setupImageproviders(QQmlApplicationEngine &engine, Qml::Inbox &clientModel)
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
