/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../"
import "../buttons"
import "../dialogs"
import "../native"
import "../windows"
import "../js/shortcut.js" as SC

FocusScope {
    id: middleColumn

    property var participantsAddresses: [] // stringlist

    function setModel(m) {
        messagesList.model = m
        messagesList.model.todoMode = Qt.binding(function() { return inbox.todoMode })
    }

    Rectangle {
        id: columnBackground
        anchors.fill: parent
        color: Style.messagesListBackground

        Item {
            id: conversationHeader
            property int _PADDING_VERTICAL: 5
            property int _PADDING_HORIZONTAL: 15

            anchors {
                left: parent.left
                top: parent.top
            }
            width: inbox.width - leftColumn.width
            height: + leftPart.implicitHeight
                    + 2*_PADDING_VERTICAL

            Row {
                id: leftPart
                anchors {
                    left: parent.left
                    top: parent.top
                    topMargin: conversationHeader._PADDING_VERTICAL
                    leftMargin: conversationHeader._PADDING_HORIZONTAL
                    rightMargin: 15
                }
                spacing: 10

                ConversationButton {
                    text: qsTr("Mark all as done")
                    tooltip: qsTr("Mark all messages in this conversation as 'done'.")

                    onClicked: messagesList.model.markAllMessagesAsReadAndDone()
                    iconSource: "/resources/scalable/state_done_g.svg"
                }

                ConversationButton {
                    text: qsTr("Change recipients")
                    tooltip: qsTr("Add or remove recipients")
                    iconSource: "/resources/scalable/change_recipients_g.svg"
                    onClicked: groupConversationChangeDialog.openDialog()

                    GroupConversationChangeDialog {
                        id: groupConversationChangeDialog
                        title: qsTr("Create new conversation")
                        initialParticipants: participantsAddresses

                        onAddressAccepted: {
                            console.info("Start conversation with '" + result + "' ...")
                            Client.addConversation(result)
                        }
                    }
                }
            }

            NativeButton {
                anchors {
                    top: parent.top
                    topMargin: conversationHeader._PADDING_VERTICAL
                    right: parent.right
                    rightMargin: conversationHeader._PADDING_HORIZONTAL
                }
                text: qsTr("Write")
                tooltip: qsTr("Write")
                         + " (%1)".arg(SC.nameOfCtrlAndKey("R", Os.osx))
                style: KulloButtonStyle {
                    iconPosition: _POSITION_LEFT
                    source: "/resources/scalable/write_w.svg"
                }
                onClicked: inbox.openAnswer()
            }
        }

        ScrollView {
            anchors {
                left: parent.left
                leftMargin: 10
                right: parent.right
                top: conversationHeader.bottom
                bottom: parent.bottom
            }
            focus: true

            onActiveFocusChanged: {
                if (activeFocus) messagesList.forceActiveFocus()
            }

            MessagesList {
                id: messagesList
            }
        }
    }
}
