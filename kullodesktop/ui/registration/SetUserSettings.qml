/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../"
import "../buttons"
import "../formelements"
import "../native"

FForm {
    id: _root

    signal done()

    anchors {
        fill: parent
    }

    validator: function () {
        return userName.text.trim() != ""
    }

    onAccepted: {
        Client.userSettings.name = userName.text
        Client.userSettings.organization = userOrganization.text
        _root.done()
    }

    onRejected: {
        userNameError.text = qsTr("Name must not be empty.")
        userName.forceActiveFocus()
    }

    Column {
        id: column1
        anchors.fill: parent
        spacing: 15

        NativeText {
            id: userNameError
            text: ""
            color: Style.red
        }

        FTextField {
            anchors {
                left: parent.left
                right: parent.right
            }
            focus: true
            id: userName
            placeholderText: qsTr("Name")
            font.pointSize: Style.fontSize.bigTextInput
        }

        FTextField {
            anchors {
                left: parent.left
                right: parent.right
            }
            id: userOrganization
            placeholderText: qsTr("Organization (optional)")
            font.pointSize: Style.fontSize.bigTextInput
        }

        Row {
            anchors {
                left: parent.left
                right: parent.right
            }
            layoutDirection: Qt.RightToLeft

            FSubmitButton {
                text: qsTr("Next")
                style: KulloButtonStyle {
                    source: "/resources/20/login_w.png"
                }
            }
        }
    }
}
