/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
                leftMargin: 5 * Hdpi.FontScalingFactor
                rightMargin: 5 * Hdpi.FontScalingFactor
                topMargin: 2 * Hdpi.FontScalingFactor
                bottomMargin: 2 * Hdpi.FontScalingFactor
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
