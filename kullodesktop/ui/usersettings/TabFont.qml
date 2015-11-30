/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../buttons"
import "../native"

TabContent {
    id: _root

    function reset() {
        fontSizeUserFactorRow.value = Devicesettings.fontSizeUserFactor
        messagesFontRow.font        = Devicesettings.messagesFont
    }

    Component.onCompleted: reset()

    Column {
        id: mainColumn
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        spacing: 10

        ComboBoxRow {
            id: fontSizeUserFactorRow
            name: qsTr("Font size")
            values: ListModel {
                //: Font size name
                ListElement { text: qsTr("small"); value: 85 }
                //: Font size name
                ListElement { text: qsTr("normal"); value: 100 }
                //: Font size name
                ListElement { text: qsTr("large"); value: 115 }
                //: Font size name
                ListElement { text: qsTr("huge"); value: 130 }
            }
            initialValue: Devicesettings.fontSizeUserFactor
            description: ""

            onValueChanged: {
                console.log("Set font factor: " + value)
                Devicesettings.fontSizeUserFactorPreview = value
            }
        }

        FontRow {
            id: messagesFontRow
            name: qsTr("Messages")

            onCurrentFontChanged: {
                Devicesettings.messagesFontPreview = currentFont
            }
        }
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
            style: KulloButtonStyle { source: "/resources/scalable/cancel_w.svg" }
            onClicked: {
                Devicesettings.fontSizeUserFactorPreview = Devicesettings.fontSizeUserFactor
                Devicesettings.messagesFontPreview = Devicesettings.messagesFont
                _root.closeWindow()
            }
        }

        NativeButton {
            id: buttonSave
            text: qsTr("Save")
            style: KulloButtonStyle { source: "/resources/scalable/ok_w.svg" }
            onClicked: {
                Devicesettings.fontSizeUserFactor = Devicesettings.fontSizeUserFactorPreview
                Devicesettings.messagesFont = Devicesettings.messagesFontPreview
                _root.closeWindow()
            }
        }
    }
}
