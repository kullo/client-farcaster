/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo 1.0
import Kullo.Visuals 1.0

import "../"
import "../native"

Item {
    property int conversationId: id_
    property bool hasSecondaryHighlight: false
    property string title: title_

    id: root
    width: 230 * Hdpi.FontScalingFactor
    height: 2 * Hdpi.FontScalingFactor
            + avatar.height
            + 2 * Hdpi.FontScalingFactor
    state: "current"

    Rectangle {
        id: row1
        height: root.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 4 * Hdpi.FontScalingFactor
        color: "transparent"
        border.color: "#eee"
        border.width: hasSecondaryHighlight ? 2 * Hdpi.FontScalingFactor : 0

        NativeImage {
            id: avatar
            width: 56 * Hdpi.FontScalingFactor
            height: 56 * Hdpi.FontScalingFactor
            anchors.verticalCenter: parent.verticalCenter
            source: "image://conversationavatars/" + Utils.urlencode(Inbox.userSettings.address) + "/" + conversationId + "?" + latestMessageTimestamp_
            asynchronous: true
            clip: true

            AdvancedRectangle {
                visible: countUndone_ > 0
                color: "#ccffffff"
                radiusTopLeft: 0
                radiusTopRight: 3 * Hdpi.FontScalingFactor
                radiusBottomRight: 0
                // 15 % of the radius (see QPainter::drawRoundedRect() and RELATIVE_CORNER_RADIUS):
                radiusBottomLeft: (avatar.width / 2) * 0.15

                anchors {
                    left: parent.left
                    bottom: parent.bottom
                }

                property int size: Math.max(countUndoneTextbox.implicitHeight,
                                            countUndoneTextbox.implicitWidth)
                width: size + 3 * Hdpi.FontScalingFactor
                height: size + 3 * Hdpi.FontScalingFactor

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
                leftMargin: 10 * Hdpi.FontScalingFactor
                rightMargin: 8 * Hdpi.FontScalingFactor
            }
            height: avatar.height
            text: root.title + (!draftEmpty_ ? " ..." : "")
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
            font.bold: countUnread_ > 0 // bold if unread message exists
            font.pointSize: Style.fontSize.conversationListTitle
            color: Style.black
            clip: true
        }
    }
}
