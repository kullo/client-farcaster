/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../native"

NativeButton {
    property url source
    property color backgroundColor: "transparent"
    property color hoverColor: "#22000000"
    property bool badgeEnabled: false
    id: root
    width: 36
    height: 36

    style: KulloButtonStyle {
        source: root.source
        paddingH: 8
        paddingV: 8
        borderRadius: 3
        backgroundColor: root.backgroundColor
        hoverColor: root.hoverColor
        badgeEnabled: root.badgeEnabled
    }
}
