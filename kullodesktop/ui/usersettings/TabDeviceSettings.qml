/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../buttons"
import "../native"

TabContent {
    id: root

    function reset() {
        updateLaneRow.value = InnerApplication.deviceSettings.updateLane
        closeToTrayRow.checked = InnerApplication.deviceSettings.closeToTray
    }

    Column {
        id: mainColumn
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        spacing: 10

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

        CheckBoxRow {
            id: closeToTrayRow
            name: qsTr("Tray icon")
            text: qsTr("Close to tray")
            checked: InnerApplication.deviceSettings.closeToTray
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
                root.closeWindowRequested()
            }
        }

        NativeButton {
            id: buttonSave
            text: qsTr("Save")
            style: KulloButtonStyle { source: "/resources/scalable/ok_w.svg" }
            onClicked: {
                InnerApplication.deviceSettings.updateLane = updateLaneRow.value
                InnerApplication.deviceSettings.closeToTray = closeToTrayRow.checked
                root.closeWindowRequested()
            }
        }
    }
}
