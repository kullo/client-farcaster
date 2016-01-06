/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import Kullo 1.0

Key {
    text: Os.osx ? "↩" : "↵"
    textPaddingTop: Os.osx ? 5 : -1

    // Override monospace font for this symbol
    fontFamily: FontList.SansSerifFamily

    TooltipArea {
        text: qsTr("Return")
    }
}
