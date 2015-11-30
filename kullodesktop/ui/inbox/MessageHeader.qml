/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../"
import "../misc"
import "../native"

Rectangle {
    property date dateSent
    property bool read
    property bool done
    property string name
    property string organization
    property string address
    property alias avatarSource: pictureFrom.source
    property bool showMessageState: true
    property bool showMessageDeliveryStatus: false
    property string messageDeliveryStatus

    property string textRow1: organization.trim() == "" ? name
                                                        : name + " (" + organization + ")"
    property string textRow2: address

    id: _header
    color: Style.messageHeaderBackground
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: _content.height

    signal avatarClicked();
    signal avatarDoubleClicked();

    Item {
        id: _content
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: pictureFrom.height + 2*pictureFrom.y

        Image {
            id: pictureFrom
            anchors.left: parent.left
            anchors.leftMargin: 5
            height: 40
            width: 40
            y: 5
            // Use source size to achieve smooth
            // scaling via c++
            sourceSize.width: width
            sourceSize.height: height

            MouseArea {
                anchors.fill: parent

                onClicked: _header.avatarClicked()
                onDoubleClicked: _header.avatarDoubleClicked()
            }
        }

        Column {
            id: senderNameBlock
            anchors {
                left: pictureFrom.right
                leftMargin: 7
                right: textDate.left
                rightMargin: 5
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
                text: textRow1
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
                text: textRow2
            }
        }

        NativeText {
            id: textDate
            anchors {
                verticalCenter: parent.verticalCenter
                right: stateBox.left
                rightMargin: 5
            }
            wrapMode: Text.NoWrap
            enabled: dateSent

            color: Style.messageHeaderForeground
            font.pointSize: Style.fontSize.messageHeaderSecondary

            Component.onCompleted: {
                // Hack to make alignment work
                horizontalAlignment = Text.AlignRight
            }

            text: {
                var now = new Date();
                var todayAtMidnight = new Date(now.getFullYear(), now.getMonth(), now.getDate())
                var yesterdayAtMidnight = new Date(now.getFullYear(), now.getMonth(), now.getDate() -1)

                if (dateSent.getTime() > todayAtMidnight.getTime())
                {
                    return dateSent.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                }
                else if (dateSent.getTime() > yesterdayAtMidnight.getTime())
                {
                    return qsTr("yesterday")
                }
                else
                {
                    return dateSent.toLocaleDateString(Qt.locale(), Locale.ShortFormat)
                }
            }

            TooltipArea {
                text: dateSent.toLocaleDateString(Qt.locale())
                      + " "
                      + dateSent.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
            }
        }

        Item {
            id: stateBox
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 5
            }
            height: 20
            width: 20
            visible: showMessageState

            Image {
                width: 20
                height: 20
                source: !read ? "/resources/20/state_unread_b.png" :
                                (!done ? "/resources/20/state_undone_b.png" : "/resources/20/state_done_b.png")
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    if (!done) markAsDone();
                    else markAsUndone();
                }
            }
        }

        Item {
            id: deliveryStatusBox
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 5
            }
            height: 20
            width: 20
            visible: showMessageDeliveryStatus

            Image {
                height: 20
                width: 20
                source: messageDeliveryStatus && messageDeliveryStatus !== "none"
                        ? "/resources/20/delivery_%1.png".arg(messageDeliveryStatus)
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
