/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import Kullo 1.0
import Kullo.Visuals 1.0

import "../"
import "../buttons"
import "../native"
import "../usersettings"
//import "../js/shortcut.js" as SC

NativeModalWindow {
    id: _root
    width: 400
    height: 500
    title: qsTr("Settings")
    objectName: "SettingsWindow"

    signal avatarChanged()

    onVisibleChanged: {
        if (visible) // window opened
        {
            if (tabUserSettings.item)
            {
                tabUserSettings.item.name         = Client.userSettings.name
                tabUserSettings.item.organization = Client.userSettings.organization
                tabUserSettings.item.address      = Client.userSettings.address
                tabUserSettings.item.footer       = Client.userSettings.footer
                tabUserSettings.item.address      = Client.userSettings.address
                tabUserSettings.item.refreshAvatar()
                checkValues()
            }
            else
            {
                console.warn("TabUserSettings not available.")
            }

            if (tabDeviceSettings.item)
            {
                tabDeviceSettings.item.reset()
            }

            if (tabFontSettings.item)
            {
                tabFontSettings.item.reset()
            }

            mainItem.forceActiveFocus()
        }
    }

    function checkValues() {
        var valid = true
        var obj = tabUserSettings.item

        if (obj.name.trim() === "") valid = false

        obj.buttonSave.enabled = valid
    }

    onClosing: {
        console.debug("UserSettingsWindow closing ...")
        Client.userSettings.discardTmpAvatar()
    }

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors {
            fill: parent
            margins: 10
        }

        Keys.onEscapePressed: _root.closeWindow()
        Keys.onPressed: handleCtrlW(event)

        TabView {
            anchors.fill: parent

            // ALT+<number> tab switching can be implemented when this bug is solved:
            // "On Ubuntu, ALT+<character> must not put <character> in text input"
            // https://bugreports.qt.io/browse/QTBUG-45698

//            Keys.onPressed: {
//                if (SC.isAltAndKey(Qt.Key_1, event))
//                {
//                    currentIndex = 0
//                    event.accepted = true
//                }
//                else if (SC.isAltAndKey(Qt.Key_2, event))
//                {
//                    currentIndex = 1
//                    event.accepted = true
//                }
//            }

            Tab {
                id: tabUserSettings
                active: true
                title: qsTr("Sender")
                source: "/usersettings/TabUserSettings.qml"
                onLoaded: {
                    item.closeWindow.connect(closeWindow)
                    item.avatarChanged.connect(avatarChanged)
                }
            }

            Tab {
                id: tabAccountSettings
                title: qsTr("Account")
                source: "/usersettings/TabAccountSettings.qml"
            }

            Tab {
                id: tabDeviceSettings
                title: qsTr("Device")
                source: "/usersettings/TabDeviceSettings.qml"
                onLoaded: {
                    item.closeWindow.connect(closeWindow)
                }
            }

            Tab {
                id: tabFontSettings
                title: qsTr("Font")
                source: "/usersettings/TabFont.qml"
                onLoaded: {
                    item.closeWindow.connect(closeWindow)
                }
            }
        }
    }
}
