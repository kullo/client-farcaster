/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import Kullo 1.0

import "../js/shortcut.js" as SC

Key {
    textPaddingTop: Os.osx ? 4 : -1
    text: SC.nameOfCtrl(Os.osx)
    fontFamily: Os.osx
                ? FontList.SansSerifFamily
                : FontList.MonospaceFamily
}
