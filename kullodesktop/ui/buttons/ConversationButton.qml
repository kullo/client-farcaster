/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../"
import "../native"

NativeButton {
    id: root

    style: KulloButtonStyle {
        iconPosition: _POSITION_LEFT
        paddingH: paddingV
        source: root.iconSource
        textColor: Style.gray
        backgroundColor: "transparent"
        hoverColor: "#55ffffff"
    }
}
