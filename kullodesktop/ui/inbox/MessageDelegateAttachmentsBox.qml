/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

import "../dialogs"

Item {
    /* public */
    property int conversationId
    property int messageId
    property bool attachmentsReady

    id: root

    implicitHeight: attachmentsList.anchors.topMargin
                    + attachmentsList.height
                    + attachmentsList.anchors.bottomMargin

    FileDialog {
        id: saveFilesDialog
        selectFolder: true
        title: qsTr("Save all")
        folder: Utils.defaultSaveAttachmentsDir()

        onAccepted: {
            console.log("Save all attachments of message " + root.messageId + " " + saveFilesDialog.fileUrl)
            attachments_.saveAllTo(saveFilesDialog.fileUrl)
        }
    }

    GridView {
        id: attachmentsList
        anchors {
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 20
            top: parent.top
            topMargin: 10
        }
        property int cols: Math.floor(width/cellWidth)
        height: cellHeight*Math.ceil(attachmentsList.count/cols)
        cellWidth: 145
        cellHeight: 85

        clip: true
        interactive: false
        currentIndex: -1

        model: attachments_
        delegate: MessageAttachmentDelegate {
            width: attachmentsList.cellWidth
            onImplicitHeightChanged: attachmentsList.cellHeight = implicitHeight

            conversationId: root.conversationId
            messageId: root.messageId
            attachmentsReady: root.attachmentsReady

            AdvancedFileSaveDialog {
                id: saveFileDialog
                filename: filename_
                title: qsTr("Save attachment")

                onAccepted: {
                    if (!attachments_.get(index).saveTo(fileUrl))
                    {
                        console.error("Error while saving file: " + fileUrl);
                    }
                }
            }

            Menu {
                id: contextMenu

                MenuItem {
                    text: qsTr('Open')
                    onTriggered: attachments_.get(index).open()
                }

                MenuItem {
                    text: qsTr('Save as')
                    onTriggered: saveFileDialog.openDialog()
                }

                MenuSeparator {
                    visible: attachments_.count() > 1
                }

                MenuItem {
                    text: qsTr('Save all')
                    onTriggered: saveFilesDialog.open()
                    visible: attachments_.count() > 1
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                propagateComposedEvents: false
                enabled: attachmentsReady_
                onClicked: {
                    selectThisMessage()
                    attachmentsList.forceActiveFocus()
                    attachmentsList.currentIndex = index

                    if (mouse.button == Qt.RightButton)
                    {
                        contextMenu.popup()
                    }
                }
                onDoubleClicked: {
                    selectThisMessage()
                    attachments_.get(index).open()
                }
            }
        }

        onCurrentIndexChanged: forceActiveFocus()
    }
}
