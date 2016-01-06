/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../dialogs"

Item {

    implicitHeight: attachmentsList.anchors.topMargin
                    + attachmentsList.height
                    + attachmentsList.anchors.bottomMargin

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
        delegate: AttachmentDelegate {
            width: attachmentsList.cellWidth
            onImplicitHeightChanged: attachmentsList.cellHeight = implicitHeight
            parentHasActiveFocus: false
            conversationId: _root.conversationId
            messageId: _root.messageId
            attachmentsReady: _root.attachmentsReady

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
                    text: qsTr('Save as')
                    onTriggered: saveFileDialog.openDialog()
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
