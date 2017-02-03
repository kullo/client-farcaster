/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    property alias name: label.text
    property alias value: input.text
    property alias readOnly: input.readOnly
    property alias enabled: input.enabled

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

    TextField {
        id: input

        anchors {
            left: label.right
            right: parent.right
        }
        y: marginTop
    }
}
