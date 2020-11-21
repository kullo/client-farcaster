/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import Kullo 1.0

import "../buttons"
import "../native"

NativeWindow {
    id: root
    title: qsTr("Sender Info")

    /* public */
    property int conversationId: -1
    property int messageId: -1
    property alias senderName: senderName.text
    property alias senderOrganization: senderOrganization.text
    property alias senderAddress: senderAddress.text

    /* private */
    property int _DEFAULT_WINDOW_WIDTH: 550 * Hdpi.FontScalingFactor
    property int _DEFAULT_WINDOW_HEIGHT: 220 * Hdpi.FontScalingFactor
    property int _LABEL_COLUMN_WIDTH: 110 * Hdpi.FontScalingFactor

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

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors.fill: parent

        Keys.onEscapePressed: root.closeWindow()
        Keys.onPressed: handleNativeWindowShortcuts(event)

        NativeImage {
            id: avatar
            anchors {
                left: parent.left
                top: parent.top
                topMargin: 10 * Hdpi.FontScalingFactor
                leftMargin: 10 * Hdpi.FontScalingFactor
            }
            source: messageId != -1
                    ? "image://messagesenderavatars/" + Utils.urlencode(Inbox.userSettings.address) + "/" + conversationId + "/" + messageId
                    : ""
            asynchronous: true
            height: 200 * Hdpi.FontScalingFactor
            width: 200 * Hdpi.FontScalingFactor
        }

        Column {
            anchors {
                top: parent.top
                left: avatar.right
                right: parent.right
                margins: 10 * Hdpi.FontScalingFactor
            }
            spacing: 10 * Hdpi.FontScalingFactor

            Item {
                width: parent.width
                height: Math.max(senderNameLabel.implicitHeight, senderName.implicitHeight)

                NativeText {
                    id: senderNameLabel
                    text: qsTr("Name") + ":"
                    font.bold: true
                    width: _LABEL_COLUMN_WIDTH
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
                    width: _LABEL_COLUMN_WIDTH
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
                    width: _LABEL_COLUMN_WIDTH
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
