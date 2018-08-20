/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import "../"
import "../native"

NativeButton {
    text: qsTr("Back")
    style: KulloButtonStyle {
        source: "/resources/scalable/arrow-rounded-left.svg"
        iconPosition: _POSITION_LEFT
        paddingH: 0
        textColor: Style.black
        backgroundColor: "transparent"
        hoverColor: "#22000000"
    }
}
