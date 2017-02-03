/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import Kullo 1.0

import "../buttons"
import "../misc"
import "../native"

NativeWindow {
    id: root
    title: qsTr("Conversation Info")

    /* public */
    property int conversationId: -1
    property alias participantsList: participantsText.text
    property var participantsAddresses
    property var participants

    /* private */
    property int _DEFAULT_WINDOW_WIDTH: 550
    property int _DEFAULT_WINDOW_HEIGHT: 220
    property Conversation _conversation: conversationId !== -1
                                         ? Inbox.conversations.get(conversationId)
                                         : null

    width: _DEFAULT_WINDOW_WIDTH
    height: _DEFAULT_WINDOW_HEIGHT

    onVisibleChanged: {
        if (visible) {
            // reset window dimensions
            root.width = _DEFAULT_WINDOW_WIDTH
            root.height = _DEFAULT_WINDOW_HEIGHT

            mainItem.forceActiveFocus()
        }
    }

    onParticipantsChanged: {
        participantsList = ""
        for (var addr in participants)
        {
            var name = participants[addr];
            if (name.trim() !== "")
            {
                participantsList += name + " (" + addr + ")\n"
            }
            else
            {
                participantsList += addr + "\n"
            }
        }
    }

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors {
            fill: parent
            margins: 10
        }

        Keys.onEscapePressed: root.closeWindow()
        Keys.onPressed: handleNativeWindowShortcuts(event)

        NativeImage {
            id: avatar
            anchors {
                left: parent.left
                top: parent.top
            }
            source: conversationId != -1 ? "image://conversationavatars/" + conversationId : ""
            height: 200
            width: 200
        }

        Column {
            anchors {
                top: parent.top
                left: avatar.right
                right: parent.right
                leftMargin: 20
            }
            spacing: 10

            NativeText {
                id: participantsLabel
                text: qsTr("Participants") + ":"
                font.bold: true
            }

            NativeSelectableText {
                id: participantsText
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }

            NativeText {
                id: statsLabel
                text: qsTr("Message counts") + ":"
                font.bold: true
            }

            Row {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: 10

                NativeSelectableText {
                    TooltipArea {
                        text: qsTr("incoming")
                    }
                    text: "%1↓".arg(_conversation ? _conversation.countIncoming : -1)
                }

                NativeSelectableText {
                    TooltipArea {
                        text: qsTr("outgoing")
                    }
                    text: "%1↑".arg(_conversation ? _conversation.countOutgoing : -1)
                }
            }
        }
    }
}
