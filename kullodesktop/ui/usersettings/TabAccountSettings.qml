/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo 1.0

import "../windows"

TabContent {
    Column {
        id: mainColumn
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        spacing: 10

        SelectableTextRow {
            id: rowAddress
            name: qsTr("Kullo address")
            value: Client.userSettings.address
        }

        ButtonRow {
            id: rowMasterKey
            name: qsTr("MasterKey")
            buttonText: qsTr("Show")
            onClicked: {
                masterKeyShowWindow.key = Client.userSettings.masterKeyPem
                masterKeyShowWindow.openWindow()
            }

            MasterKeyShowWindow {
                id: masterKeyShowWindow
                address: Client.userSettings.address
            }
        }

        ButtonRow {
            id: rowSettingsLocation
            name: qsTr("Kullo server")
            buttonText: qsTr("Open account settings")

            onClicked: settingsLocation.openUrl()

            SettingsLocation {
                id: settingsLocation
                userSettings: Client.userSettings
            }
        }
    }
}
