/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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
