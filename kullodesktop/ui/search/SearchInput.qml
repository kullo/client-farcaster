/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TextField {
    /* public */
    signal searchRequested(var query)

    /* private */
    id: root

    // height 32, radius 4 like KulloButtonStyle

    implicitWidth: 200 * Hdpi.FontScalingFactor
    height: 32 * Hdpi.FontScalingFactor
    style: TextFieldStyle {
        background: Rectangle {
            radius: 4 * Hdpi.FontScalingFactor
            color: "#55ffffff"
        }
        renderType: Text.NativeRendering
    }
    onAccepted: root.searchRequested(text)
}
