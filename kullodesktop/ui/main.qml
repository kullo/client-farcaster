/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
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
    }

    /* private */
    property real _devicePixelRatio: Screen.devicePixelRatio
    on_DevicePixelRatioChanged: {
        console.info("Device pixel ratio current screen: " + _devicePixelRatio)
    }

    property real _maxDevicePixelRatio: Hdpi.MaxDevicePixelRatio
    on_MaxDevicePixelRatioChanged: {
        console.info("Max. device pixel ratio across all screens: " + _maxDevicePixelRatio)
    }

    property real _fontScalingFactor: Hdpi.FontScalingFactor
    on_FontScalingFactorChanged:  {
        console.info("Font scaling factor: " + _fontScalingFactor)
    }

    UpdateDialog {
        id: updateDialog
        textUpdateAvailable: qsTr("A new version of Kullo is available. Download now?")

        onUpdateSilenced: {
            InnerApplication.deviceSettings.silencedUpdate = versionAvailable
        }
    }

    Connections {
        target: KulloVersionChecker
        onUpdateNotificationReceived: {
            updateDialog.versionAvailable = versionAvailable

            if (versionAvailable !== InnerApplication.deviceSettings.silencedUpdate) {
                updateDialog.state = "updateAvailable"
                updateDialog.openDialog()
            }
        }
    }

    FocusScope {
        id: app
        property alias loaderSource: appLoader.source

        Keys.onPressed: {
            if (SC.isCtrlAndKey(Qt.Key_Q, event))
            {
                event.accepted = true
                InnerApplication.quit()
            }
            else if (SC.isCtrlAndKey(Qt.Key_W, event))
            {
                event.accepted = true
                appWindow.close()
            }

            if (Os.osx) // OS X only shortcuts
            {
                if (SC.isCtrlAndKey(Qt.Key_M, event))
                {
                    event.accepted = true
                    appWindow.showMinimized()
                }
            }
        }

        anchors.fill: parent

        // States:
        // * inbox
        // * loggingIn
        // * register
        // * welcome
        // * login
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
                    loaderSource: "InboxScreen.qml"
                }

                // Known issue: Loaded only once per client start.
                // Values should be re-loaded when re-opening inbox
                //
                // Never set visibility: "Windowed" to avoid error:
                // Conflicting properties 'visible' and 'visibility' for Window 'root'
                // visible is false by default.
                // This check is bullshit:
                // https://qt.gitorious.org/qt/qtdeclarative/source/c1c7a952bc11d3bd9edcd3739c57df15b60a9e79:src/quick/items/qquickwindowmodule.cpp#L132-136
                PropertyChanges {
                    target: appWindow
                    explicit: true // do not bind properties
                    visibility: InnerApplication.deviceSettings.getMainWindowMaximized()
                                ? "Maximized"
                                : "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    explicit: true // do not bind properties
                    width: InnerApplication.deviceSettings.getMainWindowWidth()
                    height: InnerApplication.deviceSettings.getMainWindowHeight()
                    minimumWidth: InnerApplication.deviceSettings.mainWindowMinWidth
                    minimumHeight: InnerApplication.deviceSettings.mainWindowMinHeight
                }
            },
            State {
                name: "loggingIn"
                PropertyChanges {
                    target: app
                    loaderSource: "LoggingInScreen.qml"
                }
                PropertyChanges {
                    target: appWindow
                    visibility: "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    width: 1000 * Hdpi.FontScalingFactor
                    height: 600 * Hdpi.FontScalingFactor
                    minimumWidth: 800 * Hdpi.FontScalingFactor
                    minimumHeight: 400 * Hdpi.FontScalingFactor
                }
            },
            State {
                name: "register"
                PropertyChanges {
                    target: app
                    loaderSource: "RegisterScreen.qml"
                }
                PropertyChanges {
                    target: appWindow
                    visibility: "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    width: 300 * Hdpi.FontScalingFactor
                    height: 500 * Hdpi.FontScalingFactor
                    minimumWidth: 0
                    minimumHeight: 0
                }
            },
            State {
                name: "welcome"
                PropertyChanges {
                    target: app
                    loaderSource: "WelcomeScreen.qml"
                }
                PropertyChanges {
                    target: appWindow
                    visibility: "AutomaticVisibility"
                }
                PropertyChanges {
                    // accounts list width: 250
                    // right column width: 250-300
                    // height: 400-500
                    target: appWindow
                    width: (300 + (InnerApplication.deviceSettings.usersList().length > 0 ? 250 : 0)) * Hdpi.FontScalingFactor
                    height: 500 * Hdpi.FontScalingFactor
                    minimumWidth: (250 + (InnerApplication.deviceSettings.usersList().length > 0 ? 250 : 0)) * Hdpi.FontScalingFactor
                    minimumHeight: 400 * Hdpi.FontScalingFactor
                }
            },
            State {
                name: "login"
                PropertyChanges {
                    target: app
                    loaderSource: "LoginScreen.qml"
                }
                PropertyChanges {
                    target: appWindow
                    visibility: "AutomaticVisibility"
                }
                PropertyChanges {
                    target: appWindow
                    width: 300 * Hdpi.FontScalingFactor
                    height: 500 * Hdpi.FontScalingFactor
                    minimumWidth: 0
                    minimumHeight: 0
                }
            }
        ]

        state: Inbox.userSettings ? "loggingIn" : "welcome"

        onStateChanged: {
            if (state == "inbox") {
                appWindow.title = "Kullo - " + Inbox.userSettings.address
            } else {
                appWindow.title = "Kullo"
            }
        }

        Loader {
            id: appLoader
            anchors.fill: parent
            focus: true
            onLoaded: item.forceActiveFocus()
        }
    }
}
