/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

import "../native"

Item {
    property alias name: label.text
    property alias value: input.text

    property int marginTop: 0
    property int marginBottom: 0

    property int labelOffset: (input.height - label.implicitHeight)/2

    anchors {
        left: parent.left
        right: parent.right
    }

    height: marginTop + input.height + marginBottom
    implicitHeight: marginTop + input.height + marginBottom

    RowLabel {
        id: label
        y: marginTop + labelOffset
    }

    Rectangle {
        color: "#80ffffff"
        height: sizeGetter.height
        anchors {
            left: label.right
            right: parent.right
        }

        NativeSelectableText {
            id: input
            anchors {
                fill: parent
                leftMargin: 5
                rightMargin: 5
                topMargin: 2
                bottomMargin: 2
            }
            font.family: FontList.MonospaceFamily
            verticalAlignment: TextEdit.AlignVCenter
            wrapMode: TextEdit.NoWrap
        }
    }

    TextField {
        id: sizeGetter

        anchors {
            left: label.right
            right: parent.right
        }
        width: 0
        visible: false
    }
}
