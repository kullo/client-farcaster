/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import Kullo 1.0

import "../js/shortcut.js" as SC

Key {
    textPaddingTop: Os.osx
                    ? 4 * Hdpi.FontScalingFactor
                    : -1
    text: SC.nameOfCtrl(Os.osx)
    fontFamily: Os.osx
                ? FontList.SansSerifFamily
                : FontList.MonospaceFamily
}
