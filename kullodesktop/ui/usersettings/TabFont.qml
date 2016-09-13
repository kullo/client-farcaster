/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../buttons"
import "../native"

TabContent {
    id: root

    function reset() {
        fontSizeUserFactorRow.value = InnerApplication.deviceSettings.fontSizeUserFactor
        messagesFontRow.font        = InnerApplication.deviceSettings.messagesFont
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
            initialValue: InnerApplication.deviceSettings.fontSizeUserFactor
            description: ""

            onValueChanged: {
                console.log("Set font factor: " + value)
                InnerApplication.deviceSettings.fontSizeUserFactorPreview = value
            }
        }

        FontRow {
            id: messagesFontRow
            name: qsTr("Messages")

            onCurrentFontChanged: {
                InnerApplication.deviceSettings.messagesFontPreview = currentFont
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
                InnerApplication.deviceSettings.fontSizeUserFactorPreview = InnerApplication.deviceSettings.fontSizeUserFactor
                InnerApplication.deviceSettings.messagesFontPreview = InnerApplication.deviceSettings.messagesFont
                root.closeWindowRequested()
            }
        }

        NativeButton {
            id: buttonSave
            text: qsTr("Save")
            style: KulloButtonStyle { source: "/resources/scalable/ok_w.svg" }
            onClicked: {
                InnerApplication.deviceSettings.fontSizeUserFactor = InnerApplication.deviceSettings.fontSizeUserFactorPreview
                InnerApplication.deviceSettings.messagesFont = InnerApplication.deviceSettings.messagesFontPreview
                root.closeWindowRequested()
            }
        }
    }
}
