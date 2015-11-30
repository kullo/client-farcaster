/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
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
    title: qsTr("Sender Info")

    property int conversationId: -1
    property int messageId: -1
    property alias senderName: senderName.text
    property alias senderOrganization: senderOrganization.text
    property alias senderAddress: senderAddress.text

    onVisibleChanged: if (visible) mainItem.forceActiveFocus()

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors.fill: parent

        Keys.onEscapePressed: _root.closeWindow()
        Keys.onPressed: handleCtrlW(event)

        Image {
            id: avatar
            anchors {
                left: parent.left
                top: parent.top
                topMargin: 10
                leftMargin: 10
            }
            source: messageId != -1
                    ? "image://messageavatars/" + Utils.urlencode(Client.userSettings.address) + "/" + conversationId + "/" + messageId
                    : ""
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
                margins: 10
            }
            spacing: 10

            Item {
                width: parent.width
                height: Math.max(senderNameLabel.implicitHeight, senderName.implicitHeight)

                NativeText {
                    id: senderNameLabel
                    text: qsTr("Name") + ":"
                    font.bold: true
                    width: 110
                }
                NativeSelectableText {
                    id: senderName
                    anchors {
                        left: senderNameLabel.right
                        right: parent.right
                    }
                }
            }

            Item {
                width: parent.width
                height: Math.max(senderOrganizationLabel.implicitHeight, senderOrganization.implicitHeight)

                NativeText {
                    id: senderOrganizationLabel
                    text: qsTr("Organization") + ":"
                    font.bold: true
                    width: 110
                }
                NativeSelectableText {
                    id: senderOrganization
                    anchors {
                        left: senderOrganizationLabel.right
                        right: parent.right
                    }
                }
            }

            Item {
                width: parent.width
                height: Math.max(senderAddressLabel.implicitHeight, senderAddress.implicitHeight)

                NativeText {
                    id: senderAddressLabel
                    text: qsTr("Address") + ":"
                    font.bold: true
                    width: 110
                }
                NativeSelectableText {
                    id: senderAddress
                    anchors {
                        left: senderAddressLabel.right
                        right: parent.right
                    }
                }
            }
        }
    }
}
