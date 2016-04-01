/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import Kullo 1.0

import "../buttons"
import "../native"

NativeWindow {
    id: _root
    width: 550
    height: 220
    title: qsTr("Conversation Info")

    property int conversationId: -1
    property alias participantsList: participantsText.text
    property var participantsAddresses
    property var participants

    onVisibleChanged: if (visible) mainItem.forceActiveFocus()

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

        Keys.onEscapePressed: _root.closeWindow()
        Keys.onPressed: handleNativeWindowShortcuts(event)

        Image {
            id: avatar
            anchors {
                left: parent.left
                top: parent.top
            }
            source: conversationId != -1 ? "image://conversationavatars/" + conversationId : ""
            height: 200
            width: 200
            // Use source size to achieve smooth
            // scaling via c++
            sourceSize.width: width
            sourceSize.height: height
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
        }
    }
}
