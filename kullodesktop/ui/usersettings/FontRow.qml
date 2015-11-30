/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Dialogs 1.2

import "../native"

Item {
    id: _root
    property alias name: label.text
    property alias enabled: button.enabled
    property alias buttonText: button.text
    property alias currentFont: fontDialog.currentFont
    property alias font: fontDialog.font

    property int marginTop: 0
    property int marginBottom: 0

    signal clicked()

    anchors {
        left: parent.left
        right: parent.right
    }

    height: marginTop + button.height + marginBottom
    implicitHeight: marginTop + button.height + marginBottom

    RowLabel {
        id: label
        height: button.height
    }

    FontDialog {
        id: fontDialog
        modality: Qt.WindowModal

        onAccepted: {
            var family = fontDialog.font.family
            var pointSize = fontDialog.font.pointSize
            var weight = fontDialog.font.weight
            var italic = fontDialog.font.italic
            console.log("You chose: " + family + ", " + pointSize + ", " + weight + ", " + italic)
        }

        onCurrentFontChanged: {
            var family = fontDialog.font.family
            var pointSize = fontDialog.font.pointSize
            var weight = fontDialog.font.weight
            var italic = fontDialog.font.italic
            console.log("Preview: " + family + ", " + pointSize + ", " + weight + ", " + italic)
        }
    }

    NativeButton {
        id: button

        anchors {
            left: label.right
            right: parent.right
        }
        y: marginTop

        text: fontDialog.currentFont.family + " (" + fontDialog.currentFont.pointSize + ")"

        onClicked: fontDialog.open()
    }
}
