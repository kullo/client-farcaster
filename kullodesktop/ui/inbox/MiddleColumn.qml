/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

import "../"
import "../buttons"
import "../dialogs"
import "../native"
import "../search"
import "../windows"
import "../js/shortcut.js" as SC

FocusScope {
    property Conversation conversation
    property var participantsAddresses: [] // stringlist

    function selectMessage(messageId) {
        var pos = conversation.messages.find(messageId)
        messagesList.currentIndex = pos
        messagesList.positionViewAtIndex(pos, ListView.Center)
        messagesList.forceActiveFocus()
    }

    onConversationChanged: {
        if (conversation) {
            var messagesModel = conversation.messages
            messagesModel.todoMode = Qt.binding(function() { return inboxScreen.todoMode })
            messagesList.model = messagesModel
            participantsAddresses = conversation.participantsAddresses
        } else {
            messagesList.model = null
            participantsAddresses = []
        }

        search.text = ""
    }

    Rectangle {
        id: columnBackground
        anchors.fill: parent
        color: Style.messagesListBackground

        Item {
            id: conversationHeader
            property int _PADDING_VERTICAL: 5 * Hdpi.FontScalingFactor
            property int _PADDING_HORIZONTAL: 15 * Hdpi.FontScalingFactor

            anchors {
                left: parent.left
                top: parent.top
            }
            width: inboxScreen.width - leftColumn.width
            height: + leftPart.implicitHeight
                    + 2*_PADDING_VERTICAL

            Row {
                id: leftPart
                anchors {
                    left: parent.left
                    top: parent.top
                    topMargin: conversationHeader._PADDING_VERTICAL
                    leftMargin: conversationHeader._PADDING_HORIZONTAL
                    rightMargin: 15 * Hdpi.FontScalingFactor
                }
                spacing: 10 * Hdpi.FontScalingFactor

                SearchInput {
                    id: search
                    placeholderText: qsTr("Search conversation")
                    onSearchRequested: {
                        inboxScreen.openSearch(query, conversation.id, false)
                    }
                }

                ConversationButton {
                    text: qsTr("Mark all as done")
                    tooltip: qsTr("Mark all messages in this conversation as 'done'.")

                    onClicked: messagesList.model.markAllMessagesAsReadAndDone()
                    iconSource: "/resources/scalable/state_done_g.svg"
                }

                ConversationButton {
                    text: qsTr("Conversation info")
                    tooltip: qsTr("Add or remove recipients")
                    iconSource: "/resources/scalable/change_recipients_g.svg"
                    onClicked: {
                        conversationInfoWindow.conversationId = conversation.id
                        conversationInfoWindow.participantNames = conversation.participantNames
                        conversationInfoWindow.openWindow()
                        conversationInfoWindow.raise()
                        conversationInfoWindow.requestActivate()
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
                onClicked: inboxScreen.openAnswer()
            }
        }

        ScrollView {
            anchors {
                left: parent.left
                leftMargin: 10 * Hdpi.FontScalingFactor
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
