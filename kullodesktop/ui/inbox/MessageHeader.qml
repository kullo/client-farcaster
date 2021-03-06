/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../"
import "../misc"
import "../native"
import "../js/format.js" as Format

Rectangle {
    /* public */
    property date date
    property bool read
    property bool done
    property string name
    property string organization
    property string address
    property alias avatarSource: senderAvatar.source
    property bool showMessageState: true
    property bool showMessageDeliveryStatus: false
    property string messageDeliveryStatus

    /* private */
    property int _PADDING_VERTICAL: 5 * Hdpi.FontScalingFactor
    property int _PADDING_HORIZONTAL: 5 * Hdpi.FontScalingFactor
    property string _textRow1: organization.trim() == ""
                               ? name
                               : name + " (" + organization + ")"
    property string _textRow2: address

    id: root
    color: Style.messageHeaderBackground
    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }
    height: content.height

    signal avatarClicked();
    signal avatarDoubleClicked();

    Item {
        id: content
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: Math.max(senderAvatar.height, senderNameBlock.implicitHeight)
                + 2*_PADDING_VERTICAL

        Menu {
            id: contextMenu
            MenuItem {
                text: qsTr("Copy Kullo address")
                onTriggered: Utils.setClipboardText(address)
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: contextMenu.popup()
        }

        NativeImage {
            id: senderAvatar
            anchors {
                left: parent.left
                leftMargin: _PADDING_HORIZONTAL
                verticalCenter: parent.verticalCenter
            }
            height: 40 * Hdpi.FontScalingFactor
            width: 40 * Hdpi.FontScalingFactor
            asynchronous: true

            MouseArea {
                anchors.fill: parent

                onClicked: root.avatarClicked()
                onDoubleClicked: root.avatarDoubleClicked()
            }
        }

        Column {
            id: senderNameBlock
            anchors {
                left: senderAvatar.right
                leftMargin: 7 * Hdpi.FontScalingFactor
                right: textDate.left
                rightMargin: 5 * Hdpi.FontScalingFactor
                verticalCenter: parent.verticalCenter
            }

            NativeText {
                id: textboxRow1
                anchors {
                    left: parent.left
                    right: parent.right
                }
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
                color: Style.messageHeaderForeground
                font.pointSize: Style.fontSize.messageHeaderPrimary
                text: _textRow1
            }

            NativeText {
                id: textboxRow2
                anchors {
                    left: parent.left
                    right: parent.right
                }
                wrapMode: Text.NoWrap
                elide: Text.ElideMiddle
                color: Style.messageHeaderForegroundSecondary
                font.pointSize: Style.fontSize.messageHeaderSecondary
                visible: text != ""
                text: _textRow2
            }
        }

        NativeText {
            id: textDate
            anchors {
                verticalCenter: parent.verticalCenter
                right: stateBox.left
                rightMargin: 5 * Hdpi.FontScalingFactor
            }
            wrapMode: Text.NoWrap
            enabled: date

            color: Style.messageHeaderForeground
            font.pointSize: Style.fontSize.messageHeaderSecondary

            Component.onCompleted: {
                // Hack to make alignment work
                horizontalAlignment = Text.AlignRight
            }

            text: Format.humanDatetime(date, Locale.ShortFormat, Locale.ShortFormat)

            TooltipArea {
                text: Format.humanFullDatetime(date, Locale.LongFormat, Locale.ShortFormat)
            }
        }

        Item {
            id: stateBox
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: _PADDING_HORIZONTAL
            }
            height: 20 * Hdpi.FontScalingFactor
            width: 20 * Hdpi.FontScalingFactor
            visible: showMessageState


            NativeImage {
                width: 20 * Hdpi.FontScalingFactor
                height: 20 * Hdpi.FontScalingFactor
                source: !read
                        ? "/resources/scalable/state_unread_b.svg"
                        : (!done ? "/resources/scalable/state_undone_b.svg" : "/resources/scalable/state_done_b.svg")
            }

            TooltipArea {
                anchors.fill: parent
                text: done
                      ? qsTr("Mark as undone")
                      : qsTr("Mark as done")

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (!done) markAsDone();
                        else markAsUndone();
                    }
                }
            }
        }

        Item {
            id: deliveryStatusBox
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: _PADDING_HORIZONTAL
            }
            height: 20 * Hdpi.FontScalingFactor
            width: 20 * Hdpi.FontScalingFactor
            visible: showMessageDeliveryStatus

            NativeImage {
                height: 20 * Hdpi.FontScalingFactor
                width: 20 * Hdpi.FontScalingFactor
                source: messageDeliveryStatus && messageDeliveryStatus !== "none"
                        ? "/resources/scalable/delivery_%1.svg".arg(messageDeliveryStatus)
                        : ""

                TooltipArea {
                    enabled: messageDeliveryStatus && messageDeliveryStatus !== "none"
                    text: {
                        if (messageDeliveryStatus && messageDeliveryStatus !== "none")
                        {
                            if (messageDeliveryStatus === "sending") return qsTr("Sending …")
                            if (messageDeliveryStatus === "sent")    return qsTr("Sent sucessfully")
                            if (messageDeliveryStatus === "error")   return qsTr("Error during send process")
                        }
                        else
                        {
                            return ""
                        }
                    }
                }
            }
        }
    }
}
