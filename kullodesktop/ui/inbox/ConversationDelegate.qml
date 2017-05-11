/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo 1.0
import Kullo.Visuals 1.0

import "../"
import "../native"

Item {
    property int conversationId: id_
    property bool hasSecondaryHighlight: false

    id: root
    width: 230
    height: 2 + avatar.height + 2
    state: "current"

    Rectangle {
        id: row1
        height: root.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 4
        color: "transparent"
        border.color: "#eee"
        border.width: hasSecondaryHighlight ? 2 : 0

        NativeImage {
            id: avatar
            width: 56
            height: 56
            anchors.verticalCenter: parent.verticalCenter
            source: "image://conversationavatars/" + Utils.urlencode(Inbox.userSettings.address) + "/" + conversationId + "?" + latestMessageTimestamp_
            asynchronous: true
            clip: true

            AdvancedRectangle {
                visible: countUndone_ > 0
                color: "#ccffffff"
                radiusTopLeft: 0
                radiusTopRight: 3
                radiusBottomRight: 0
                // See QPainter::drawRoundedRect():
                // (56/2) * 0.15 = 4.2
                radiusBottomLeft: 4.2

                anchors {
                    left: parent.left
                    bottom: parent.bottom
                }

                property int size: Math.max(countUndoneTextbox.implicitHeight,
                                            countUndoneTextbox.implicitWidth)
                width: size + 3
                height: size + 3

                NativeText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    id: countUndoneTextbox
                    text: countUndone_
                    font.bold: countUnread_ > 0 // bold if unread message exists
                    font.pointSize: Style.fontSize.conversationListCounter
                    color: Style.gray
                }
            }
        }

        NativeText {
            id: textBox
            anchors {
                left: avatar.right
                right: parent.right
                leftMargin: 10
                rightMargin: 8
            }
            height: avatar.height
            text: participantsList_
                  + (!draftEmpty_ ? " ..." : "")
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
            font.bold: countUnread_ > 0 // bold if unread message exists
            font.pointSize: Style.fontSize.conversationListTitle
            color: Style.black
            clip: true
        }
    }
}
