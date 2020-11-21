/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

import Kullo 1.0

Key {
    text: Os.osx ? "↩" : "↵"
    textPaddingTop: Os.osx
                    ? 5 * Hdpi.FontScalingFactor
                    : -1

    // Override monospace font for this symbol
    fontFamily: FontList.SansSerifFamily

    TooltipArea {
        text: qsTr("Return")
    }
}
