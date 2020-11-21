/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo 1.0
import Kullo.Visuals 1.0

import "../"
import "../buttons"
import "../native"
import "../usersettings"

NativeModalWindow {
    title: qsTr("Settings")
    objectName: "SettingsWindow"

    /* private */
    id: root
    property int _DEFAULT_WINDOW_WIDTH: 400 * Hdpi.FontScalingFactor
    property int _DEFAULT_WINDOW_HEIGHT: 500 * Hdpi.FontScalingFactor

    width: _DEFAULT_WINDOW_WIDTH
    height: _DEFAULT_WINDOW_HEIGHT

    onVisibleChanged: {
        if (visible) // window opened
        {
            reloadDataIntoUi()

            // reset window dimensions
            root.width = _DEFAULT_WINDOW_WIDTH
            root.height = _DEFAULT_WINDOW_HEIGHT

            mainItem.forceActiveFocus()
        }
    }

    onClosing: resetTemporaryValues()

    function reloadDataIntoUi() {
        fontSizeUserFactorRow.value = InnerApplication.deviceSettings.fontSizeUserFactor
        messagesFontRow.font = InnerApplication.deviceSettings.messagesFont
        closeToTrayRow.checked = InnerApplication.deviceSettings.closeToTray
        playSoundRow.checked = !InnerApplication.deviceSettings.muted
        updateLaneRow.value = InnerApplication.deviceSettings.updateLane
    }

    function resetTemporaryValues() {
        InnerApplication.deviceSettings.fontSizeUserFactorPreview = InnerApplication.deviceSettings.fontSizeUserFactor
        InnerApplication.deviceSettings.messagesFontPreview = InnerApplication.deviceSettings.messagesFont
    }

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors {
            fill: parent
            margins: 10 * Hdpi.FontScalingFactor
        }

        Keys.onEscapePressed: root.closeWindow()
        Keys.onPressed: handleNativeWindowShortcuts(event)

        Column {
            id: mainColumn
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            spacing: 10 * Hdpi.FontScalingFactor

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

            CheckBoxRow {
                id: closeToTrayRow
                name: qsTr("Tray icon")
                text: qsTr("Close to tray")
                checked: InnerApplication.deviceSettings.closeToTray
            }

            CheckBoxRow {
                id: playSoundRow
                name: qsTr("Notifications")
                text: qsTr("Sound for new messages")
                checked: !InnerApplication.deviceSettings.muted
            }

            ComboBoxRow {
                id: updateLaneRow
                name: qsTr("Update lane")
                values: ListModel {
                    //: Update lane name
                    ListElement { text: qsTr("important"); value: "important" }
                    //: Update lane name
                    ListElement { text: qsTr("all"); value: "all" }
                }
                initialValue: InnerApplication.deviceSettings.updateLane
                description: qsTr("'Important' updates are those we think everyone should install, e.g. security fixes or mayor feature improvements.") + " "
                             + qsTr("We plan to have an 'important' update every 4-8 weeks.") + " "
                             + qsTr("'All' updates contain minor improvements or changes that are only relevant for some users. Those come once a week or even more often.") + " "
                             + qsTr("This is for pioneers.")
            }
        }

        Row {
            id: bottonsRow
            anchors {
                right: parent.right
                bottom: parent.bottom
            }
            spacing: 10 * Hdpi.FontScalingFactor

            NativeButton {
                id: buttonDiscard
                text: qsTr("Discard")
                style: KulloButtonStyle { source: "/resources/scalable/cancel_w.svg" }
                onClicked: {
                    root.closeWindow()
                }
            }

            NativeButton {
                id: buttonSave
                text: qsTr("Save")
                style: KulloButtonStyle { source: "/resources/scalable/ok_w.svg" }
                onClicked: {
                    InnerApplication.deviceSettings.fontSizeUserFactor = InnerApplication.deviceSettings.fontSizeUserFactorPreview
                    InnerApplication.deviceSettings.messagesFont = InnerApplication.deviceSettings.messagesFontPreview
                    InnerApplication.deviceSettings.updateLane = updateLaneRow.value
                    InnerApplication.deviceSettings.closeToTray = closeToTrayRow.checked
                    InnerApplication.deviceSettings.muted = !playSoundRow.checked

                    root.closeWindow()
                }
            }
        }
    }
}
