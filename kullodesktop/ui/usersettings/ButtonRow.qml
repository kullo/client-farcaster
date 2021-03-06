/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

import "../native"

Item {
    id: _root
    property alias name: label.text
    property alias enabled: button.enabled
    property alias buttonText: button.text

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

    NativeButton {
        id: button

        anchors {
            left: label.right
            right: parent.right
        }
        y: marginTop

        onClicked: _root.clicked()
    }
}
