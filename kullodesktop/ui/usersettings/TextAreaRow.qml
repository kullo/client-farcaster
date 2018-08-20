/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    property alias name: label.text
    property alias value: input.text

    property int marginTop: 0
    property int marginBottom: 0
    property int labelOffset: 0

    anchors {
        left: parent.left
        right: parent.right
    }

    implicitHeight: marginTop + input.implicitHeight + marginBottom

    RowLabel {
        id: label
        y: marginTop + labelOffset
    }

    TextArea {
        id: input

        anchors {
            top: parent.top
            left: label.right
            right: parent.right
            bottom: parent.bottom
        }
        y: marginTop
        height: 100
    }
}
