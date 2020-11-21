/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

Button {
    Keys.onPressed: {
        // Key_Enter always comes with KeypadModifier
        if (!Os.osx
                && (event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
                && (event.modifiers === Qt.NoModifier || event.modifiers === Qt.KeypadModifier))
        {
            event.accepted = true
            clicked()
        }
    }
}
