/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

Item {
    /* public */
    property int conversationId
    property int messageId
    property bool attachmentsReady
    property var attachments
    signal saveAttachmentRequested(var attachment)
    signal saveAttachmentsRequested(var attachments)

    id: root

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

        model: attachments
        delegate: MessageAttachmentDelegate {
            width: attachmentsList.cellWidth
            onImplicitHeightChanged: attachmentsList.cellHeight = implicitHeight

            conversationId: root.conversationId
            messageId: root.messageId
            attachmentsReady: root.attachmentsReady

            Menu {
                id: contextMenu

                MenuItem {
                    text: qsTr('Open')
                    onTriggered: {
                        var attachment = attachments.get(index)
                        if (!attachment.openAsync()) {
                            inboxScreen.showOpenFileError(attachment.filename)
                        }
                    }
                }

                MenuItem {
                    text: qsTr('Save as')
                    onTriggered: {
                        root.saveAttachmentRequested(attachments.get(index))
                    }
                }

                MenuSeparator {
                    visible: attachments.count() > 1
                }

                MenuItem {
                    text: qsTr('Save all')
                    visible: attachments.count() > 1
                    onTriggered: {
                        root.saveAttachmentsRequested(attachments)
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                propagateComposedEvents: false
                enabled: attachmentsReady
                onClicked: {
                    selectThisMessage()
                    attachmentsList.forceActiveFocus()
                    attachmentsList.currentIndex = index

                    if (mouse.button === Qt.RightButton)
                    {
                        contextMenu.popup()
                    }
                }
                onDoubleClicked: {
                    selectThisMessage()

                    var attachment = attachments.get(index)
                    if (!attachment.openAsync()) {
                        inboxScreen.showOpenFileError(attachment.filename)
                    }
                }
            }
        }

        onCurrentIndexChanged: forceActiveFocus()
    }
}
