/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import Kullo 1.0
import Kullo.Visuals 1.0

import "../"
import "../buttons"
import "../native"
import "../windows"

TabContent {
    id: root

    /* public */
    property string address
    property alias name: rowName.value
    property alias organization: rowOrganization.value
    property alias footer: rowFooter.value

    function refreshAvatar() {
        rowAvatar.refreshAvatar()
    }

    property alias buttonSave: buttonSave

    signal avatarChanged()

    Column {
        id: mainColumn
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        spacing: 10

        TextFieldRow {
            id: rowName
            name: qsTr("Name")
            height: implicitHeight + rowNameHint.height

            Hint {
                id: rowNameHint
                anchors {
                    right: parent.right
                    rightMargin: 20
                    bottom: parent.bottom
                }
                height: rowName.value.trim() == "" ? 30 : 0
                width: rowNameHintText.implicitWidth
                       + 2*rowNameHintText.anchors.leftMargin
                paddingTop: 10
                color: Style.errorBannerBackground
                clip: true

                NativeText {
                    id: rowNameHintText
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 2
                        left: parent.left
                        leftMargin: 10
                    }
                    color: Style.errorBannerTextColor
                    text: qsTr("Name must not be empty.")
                }
            }

            onValueChanged: checkValues()
        }

        TextFieldRow {
            id: rowOrganization
            name: qsTr("Organization")
        }

        AvatarRow {
            id: rowAvatar
            name: qsTr("Avatar")
            address: root.address
            labelOffset: rowOrganization.labelOffset
            onAvatarChanged: root.avatarChanged()
        }
    }

    Item {
        anchors {
            top: mainColumn.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        TextAreaRow {
            id: rowFooter
            name: qsTr("Imprint")
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: bottonsRow.top
                topMargin: 10
                bottomMargin: 10
            }
            labelOffset: rowOrganization.labelOffset
        }

        Row {
            id: bottonsRow
            anchors {
                right: parent.right
                bottom: parent.bottom
            }
            spacing: 10

            NativeButton {
                id: buttonDiscard
                text: qsTr("Discard")
                style: KulloButtonStyle {
                    source: "/resources/scalable/cancel_w.svg"
                }
                onClicked: root.closeWindowRequested()
            }

            NativeButton {
                id: buttonSave
                text: qsTr("Save")
                style: KulloButtonStyle {
                    source: "/resources/scalable/ok_w.svg"
                }
                onClicked: {
                    if (Client.userSettings.name !== rowName.value)
                        Client.userSettings.name = rowName.value
                    if (Client.userSettings.organization !== rowOrganization.value)
                        Client.userSettings.organization = rowOrganization.value
                    if (Client.userSettings.footer !== rowFooter.value)
                        Client.userSettings.footer = rowFooter.value

                    if (Client.userSettings.storeTmpAvatar()) {
                        root.avatarChanged()
                    }

                    root.closeWindowRequested()
                }
            }
        }
    }
}
