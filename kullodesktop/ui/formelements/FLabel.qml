/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3

Label {
    property var target: null
    property bool _FORM_TYPE_IGNORE_IN_FORM_VALIDATION: true
    property bool _FORM_TYPE_IGNORE_IN_FORM_CLEARING: true

    width: 100 * Hdpi.FontScalingFactor
    height: target
            ? target.implicitHeight
            : 62 * Hdpi.FontScalingFactor
    verticalAlignment: Text.AlignVCenter

    MouseArea {
        anchors.fill: parent
        onClicked: target.forceActiveFocus()
    }
}
