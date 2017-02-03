/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "buttons"
import "formelements"
import "misc"
import "native"

FocusScope {
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: Style.beigeLight

        Logo {
            id: logo
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 10
                rightMargin: 10
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
            spacing: 30

            Column {
                id: blockOpen
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: 10
                visible: InnerApplication.deviceSettings.usersList().length > 0

                NativeText {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: qsTr("Open inbox")
                }

                FForm {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: Math.max(userSelection.implicitHeight, openInboxButton.implicitHeight)
                    focus: true

                    onAccepted: {
                        var loginAddress = userSelection.currentText
                        Inbox.loadCredentials(loginAddress)
                        app.state = "loggingIn"
                    }

                    FComboBox {
                        property var userList: InnerApplication.deviceSettings.usersList()
                        id: userSelection
                        anchors {
                            left: parent.left
                            right: openInboxButton.left
                            rightMargin: 10
                        }
                        model: userList

                        Component.onCompleted: {
                            currentIndex = find(InnerApplication.deviceSettings.lastActiveUser);

                            if (currentIndex == -1 && InnerApplication.deviceSettings.usersList().length > 0)
                            {
                                currentIndex = 0
                            }
                        }
                    }

                    FSubmitButton {
                        id: openInboxButton
                        anchors {
                            right: parent.right
                        }
                        text: qsTr("Open")
                        style: KulloButtonStyle {}
                    }
                }
            }

            Column {
                id: blockLogin
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: 10

                NativeText {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: InnerApplication.deviceSettings.usersList().length !== 0
                          ? qsTr("Login with another account.")
                          : qsTr("Login with your Kullo credentials.")
                }

                FForm {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: loginButton.implicitHeight

                    onAccepted: app.state = "login"

                    FSubmitButton {
                        id: loginButton
                        anchors {
                            right: parent.right
                        }
                        text: qsTr("Login")
                        style: KulloButtonStyle {
                            source: "/resources/scalable/login_w.svg"
                        }
                    }
                }
            }

            Column {
                id: blockRegister
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: 10

                NativeText {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: qsTr("Register, if you don't have a Kullo address yet.")
                }

                FForm {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: registerButton.implicitHeight

                    onAccepted: app.state = "register"

                    FSubmitButton {
                        id: registerButton
                        anchors {
                            right: parent.right
                        }
                        text: qsTr("Register")
                        style: KulloButtonStyle { }
                    }
                }
            }
        }
    }
}
