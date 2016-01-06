/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Window 2.2

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "buttons"
import "misc"
import "native"

FocusScope {
    id: root
    anchors.fill: parent

    function validateName() {
        return userName.text.trim() != ""
    }

    Rectangle {
        anchors.fill: parent
        color: Style.baigeLight

        Logo {
            id: logo
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 10
                rightMargin: anchors.topMargin
            }
        }

        Column {
            id: column1
            width: 260
            anchors {
                top: logo.bottom
                topMargin: 50
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                leftMargin: 20
                rightMargin: 20
            }
            spacing: 15

            BackButton {
                onClicked: app.state = "login"
            }

            NativeText {
                id: userNameError
                text: ""
                color: Style.red
            }

            TextField {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                id: userName
                placeholderText: qsTr("Name")
                font.pointSize: Style.fontSize.bigTextInput
                focus: true
            }

            TextField {
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

                NativeButton {
                    text: qsTr("Next")
                    style: KulloButtonStyle {
                        source: "/resources/scalable/login_w.svg"
                    }
                    onClicked: {
                        if (!validateName())
                        {
                            userNameError.text = qsTr("Name must not be empty.")
                            userName.forceActiveFocus()
                        }
                        else
                        {
                            Client.userSettings.name = userName.text
                            Client.userSettings.organization = userOrganization.text
                            app.state = "loggingIn"
                        }
                    }
                }
            }
        }
    }
}
