/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "qmlsetup.h"

#include <QQmlApplicationEngine>

#include <apimirror/Client.h>
#include <apimirror/enums/DraftPartHolder.h>
#include <apimirror/enums/LocalErrorHolder.h>
#include <apimirror/enums/NetworkErrorHolder.h>
#include <apimirror/enums/SyncPhaseHolder.h>

#include "kullodesktop/imageproviders/attachmentpreviewprovider.h"
#include "kullodesktop/imageproviders/conversationavatarprovider.h"
#include "kullodesktop/imageproviders/messagesenderavatarprovider.h"
#include "kullodesktop/imageproviders/participantavatarprovider.h"
#include "kullodesktop/imageproviders/usersettingsavatarprovider.h"

#include "kullodesktop/qml/attachmentlistmodel.h"
#include "kullodesktop/qml/attachmentmodel.h"
#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/conversationlistsorted.h"
#include "kullodesktop/qml/conversationlistsortedfiltered.h"
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
#include "kullodesktop/qml_components/accountinfo.h"
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
#include "kullodesktop/qml_components/searchmodel.h"
#include "kullodesktop/qml_components/stabletimer.h"

namespace KulloDesktop {
namespace QmlBridge {

void QmlSetup::setupTypes()
{
    // Components to be instantiated by Qml
    qmlRegisterType<FileSaveDialog>("Kullo", 1, 0, "FileSaveDialog");
    qmlRegisterType<FileOpenDialog>("Kullo", 1, 0, "FileOpenDialog");
    qmlRegisterType<QmlComponents::AccountInfo>("Kullo", 1, 0, "AccountInfo");
    qmlRegisterType<QmlComponents::AttachmentsAdder>("Kullo", 1, 0, "AttachmentsAdder");
    qmlRegisterType<QmlComponents::ChallengeTypes>("Kullo", 1, 0, "ChallengeTypes");
    qmlRegisterType<QmlComponents::Changelog>("Kullo", 1, 0, "Changelog");
    qmlRegisterType<QmlComponents::ExistenceChecker>("Kullo", 1, 0, "ExistenceChecker");
    qmlRegisterType<QmlComponents::KulloUpdaterModel>("Kullo", 1, 0, "KulloUpdater");
    qmlRegisterType<QmlComponents::LibraryLoggerQmlConnector>("Kullo", 1, 0, "LibraryLoggerQmlConnector");
    qmlRegisterType<QmlComponents::LoginChecker>("Kullo", 1, 0, "LoginChecker");
    qmlRegisterType<QmlComponents::Registerer>("Kullo", 1, 0, "Registerer");
    qmlRegisterType<QmlComponents::SearchModel>("Kullo", 1, 0, "SearchModel");
    qmlRegisterType<QmlComponents::Scheduler>("Kullo", 1, 0, "Scheduler");
    qmlRegisterType<QmlComponents::StableTimer>("Kullo", 1, 0, "StableTimer");
    qmlRegisterType<QmlComponents::Visuals::AdvancedRectangle>("Kullo.Visuals", 1, 0, "AdvancedRectangle");
    qmlRegisterType<QmlComponents::Visuals::Hint>("Kullo.Visuals", 1, 0, "Hint");

    qmlRegisterType<Qml::AttachmentListModel>("Kullo", 1, 0, "AttachmentList");
    qmlRegisterType<Qml::AttachmentModel>("Kullo", 1, 0, "Attachment");
    qmlRegisterType<Qml::ConversationListSorted>("Kullo", 1, 0, "ConversationListSorted");
    qmlRegisterType<Qml::ConversationListSortedFiltered>("Kullo", 1, 0, "ConversationListSortedFiltered");
    qmlRegisterType<Qml::ConversationModel>("Kullo", 1, 0, "Conversation");
    qmlRegisterType<Qml::DeviceSettings>("Kullo", 1, 0, "DeviceSettings");
    qmlRegisterType<Qml::DraftModel>("Kullo", 1, 0, "Draft");
    qmlRegisterType<Qml::DraftAttachmentListModel>("Kullo", 1, 0, "DraftAttachmentList");
    qmlRegisterType<Qml::DraftAttachmentModel>("Kullo", 1, 0, "DraftAttachment");
    qmlRegisterType<Qml::MessageListModel>("Kullo", 1, 0, "MessageList");
    qmlRegisterType<Qml::MessageModel>("Kullo", 1, 0, "Message");
    qmlRegisterType<Qml::Sender>("Kullo", 1, 0, "Sender");

    qmlRegisterUncreatableType<Qml::UserSettings>("Kullo", 1, 0, "UserSettings", "Do not create in QML");

    // Enums
    qmlRegisterType<ApiMirror::Enums::DraftPartHolder>("Kullo", 1, 0, "DraftPart");
    qmlRegisterType<ApiMirror::Enums::LocalErrorHolder>("Kullo", 1, 0, "LocalError");
    qmlRegisterType<ApiMirror::Enums::NetworkErrorHolder>("Kullo", 1, 0, "NetworkError");
    qmlRegisterType<ApiMirror::Enums::SyncPhaseHolder>("Kullo", 1, 0, "SyncPhase");
    ApiMirror::Enums::DraftPartHolder::registerEnumsInClassForSignalSlot();
    ApiMirror::Enums::LocalErrorHolder::registerEnumsInClassForSignalSlot();
    ApiMirror::Enums::NetworkErrorHolder::registerEnumsInClassForSignalSlot();
    ApiMirror::Enums::SyncPhaseHolder::registerEnumsInClassForSignalSlot();

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
    engine.addImageProvider(QStringLiteral("messagesenderavatars"), new MessageSenderAvatarProvider(clientModel));
    engine.addImageProvider(QStringLiteral("participantavatars"), new ParticipantAvatarProvider(clientModel));
    engine.addImageProvider(QStringLiteral("usersettingsavatars"), new UserSettingsAvatarProvider(clientModel));
}

}
}
