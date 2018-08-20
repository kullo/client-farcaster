/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "buttons"
import "formelements"
import "misc"
import "native"
import "welcome"

FocusScope {
    anchors.fill: parent

    Rectangle {
        id: leftColumn
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: InnerApplication.deviceSettings.usersList().length > 0 ? 250 : 0
        color: Style.welcomeAccountsBackground

        AccountsList {
            anchors.fill: parent
            focus: true
        }
    }

    Rectangle {
        id: rightColumn
        anchors {
            left: leftColumn.right
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
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
            anchors {
                top: logo.bottom
                topMargin: 60
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                leftMargin: 20
                rightMargin: 20
            }
            spacing: 40

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
                            left: parent.left
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
                            left: parent.left
                        }
                        text: qsTr("Register")
                        style: KulloButtonStyle {
                            source: "/resources/scalable/star_w.svg"
                        }
                    }
                }
            }
        }
    }
}
