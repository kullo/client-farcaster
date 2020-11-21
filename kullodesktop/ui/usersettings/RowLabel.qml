/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

import "../native"

NativeText {
    property alias text: label.text

    id: label

    anchors {
        left: parent.left
    }
    width: 100 * Hdpi.FontScalingFactor

    verticalAlignment: Text.AlignVCenter
}
