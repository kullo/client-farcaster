/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

TextEdit {
    id: root
    renderType: Text.NativeRendering
    selectByMouse: true
    clip: true
    wrapMode: TextEdit.WordWrap

    SystemPalette {
        id: palette
        colorGroup: root.enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    color: palette.text
    selectionColor: palette.highlight
    selectedTextColor: palette.highlightedText
}
