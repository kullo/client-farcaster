/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

import "../native"

NativeButton {
    property url source
    property color backgroundColor: "transparent"
    property color hoverColor: "#22000000"
    property bool badgeEnabled: false
    id: root
    width: 36 * Hdpi.FontScalingFactor
    height: 36 * Hdpi.FontScalingFactor

    style: KulloButtonStyle {
        source: root.source
        paddingH: 8 * Hdpi.FontScalingFactor
        paddingV: 8 * Hdpi.FontScalingFactor
        borderRadius: 3 * Hdpi.FontScalingFactor
        backgroundColor: root.backgroundColor
        hoverColor: root.hoverColor
        badgeEnabled: root.badgeEnabled
    }
}
