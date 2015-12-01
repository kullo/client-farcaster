/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../"
import "../native"

NativeButton {
    text: qsTr("Back")
    style: KulloButtonStyle {
        source: "/resources/20/arrow-rounded-left.png"
        iconPosition: _POSITION_LEFT
        paddingH: 0
        textColor: Style.black
        backgroundColor: "transparent"
        hoverColor: "#22000000"
    }
}
