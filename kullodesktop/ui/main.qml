/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import Kullo 1.0

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "dialogs"
import "js/shortcut.js" as SC

ApplicationWindow {
    id: appWindow
    title: "Kullo"
    color: Style.messagesListBackground

    Component.onCompleted: {
        app.forceActiveFocus()
        KulloVersionChecker.run()
    }

    UpdateDialog {
        id: updateDialog
        textUpdateAvailable: qsTr("A new version of Kullo is available. Download now?")
    }

    Connections {
        target: KulloVersionChecker
        onUpdateAvailable: {
            updateDialog.state = "updateAvailable"
            updateDialog.openDialog();
        }
    }

    FocusScope {
        id: app
        property alias loaderSource: appLoader.source

        Keys.onPressed: {
            if (SC.isCtrlAndKey(Qt.Key_Q, event))
            {
                event.accepted = true
                Qt.quit()
            }
            else if (SC.isCtrlAndKey(Qt.Key_W, event))
            {
                event.accepted = true
                appWindow.hide()
            }
        }

        anchors.fill: parent

        // States:
        // * inbox
        // * loggingIn
        // * register
        // * welcome
        // * login
        // * setUserSettings
        // * registerSetUserSettings
        //
        // Properties to be set in every state
        // * app.loaderSource
        // * appWindow.visibility
        // * appWindow.width
        // * appWindow.height
        // * appWindow.minimumWidth
        // * appWindow.minimumHeight
        states: [
            State {
                name: "inbox"
                PropertyChanges {
                    target: app
                    loaderSource: "viewInbox.qml"
                }

                // Known issue: Loaded only once per client start.
                // Values should be re-loaded when re-opening inbox
                //
                // Never set visibility: Windowed to avoid error:
                // Conflicting properties 'visible' and 'visibility' for Window 'root'
                // visible is false by default.
                // This check is bullshit:
                // https://qt.gitorious.org/qt/qtdeclarative/source/c1c7a952bc11d3bd9edcd3739c57df15b60a9e79:src/quick/items/qquickwindowmodule.cpp#L132-136
                PropertyChanges {
                    target: appWindow
                    visibility: Devicesettings.getMainWindowMaximized()
                                ? "Maximized"
                                : "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    width: Devicesettings.getMainWindowWidth()
                    height: Devicesettings.getMainWindowHeight()
                    minimumWidth: Devicesettings.mainWindowMinWidth
                    minimumHeight: Devicesettings.mainWindowMinHeight
                }
            },
            State {
                name: "loggingIn"
                PropertyChanges {
                    target: app
                    loaderSource: "viewLoggingIn.qml"
                }
                PropertyChanges {
                    target: appWindow
                    visibility: "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    width: 1000
                    height: 600
                    minimumWidth: 800
                    minimumHeight: 400
                }
            },
            State {
                name: "register"
                PropertyChanges {
                    target: app
                    loaderSource: "viewRegister.qml"
                }
                PropertyChanges {
                    target: appWindow
                    visibility: "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    width: 300
                    height: 500
                    minimumWidth: 0
                    minimumHeight: 0
                }
            },
            State {
                name: "welcome"
                PropertyChanges {
                    target: app
                    loaderSource: "viewWelcome.qml"
                }
                PropertyChanges {
                    target: appWindow
                    visibility: "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    width: 300
                    height: 500
                    minimumWidth: 0
                    minimumHeight: 0
                }
            },
            State {
                name: "login"
                PropertyChanges {
                    target: app
                    loaderSource: "viewLogin.qml"
                }
                PropertyChanges {
                    target: appWindow
                    visibility: "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    width: 300
                    height: 500
                    minimumWidth: 0
                    minimumHeight: 0
                }
            },
            State {
                name: "setUserSettings"
                PropertyChanges {
                    target: app
                    loaderSource: "viewLoginSetUserSettings.qml"
                }
                PropertyChanges {
                    target: appWindow
                    visibility: "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    width: 300
                    height: 500
                    minimumWidth: 0
                    minimumHeight: 0
                }
            },
            State {
                name: "registerSetUserSettings"
                PropertyChanges {
                    target: app
                    loaderSource: "viewRegisterSetUserSettings.qml"
                }
                PropertyChanges {
                    target: appWindow
                    visibility: "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    width: 300
                    height: 500
                    minimumWidth: 0
                    minimumHeight: 0
                }
            }
        ]

        state: Client.userSettings.loginCredentialsStored() ? "loggingIn" : "welcome"

        Loader {
            id: appLoader
            anchors.fill: parent
            focus: true
            onLoaded: item.forceActiveFocus()
        }
    }
}
