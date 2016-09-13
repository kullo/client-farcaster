/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import Kullo 1.0
import Kullo.Visuals 1.0

import "../"
import "../buttons"
import "../native"
import "../usersettings"

NativeModalWindow {
    id: root
    width: 400
    height: 500
    title: qsTr("Settings")
    objectName: "SettingsWindow"

    // unused signal since sender avatar is not shown in compose view anymore
    signal avatarChanged()

    onVisibleChanged: {
        if (visible) // window opened
        {
            if (tabUserSettings.item)
            {
                tabUserSettings.item.address      = Inbox.userSettings.address
                tabUserSettings.item.name         = Inbox.userSettings.name
                tabUserSettings.item.organization = Inbox.userSettings.organization
                tabUserSettings.item.footer       = Inbox.userSettings.footer
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
        Inbox.userSettings.discardTmpAvatar()
    }

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors {
            fill: parent
            margins: 10
        }

        Keys.onEscapePressed: root.closeWindow()
        Keys.onPressed: handleNativeWindowShortcuts(event)

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
                    item.avatarChanged.connect(avatarChanged)
                    item.closeWindowRequested.connect(closeWindow)
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
                    item.closeWindowRequested.connect(closeWindow)
                }
            }

            Tab {
                id: tabFontSettings
                title: qsTr("Font")
                source: "/usersettings/TabFont.qml"
                onLoaded: {
                    item.closeWindowRequested.connect(closeWindow)
                }
            }
        }
    }
}
