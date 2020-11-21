/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

import ".."

NativeTextEdit {
    id: root
    height: implicitHeight

    textFormat: Text.PlainText
    readOnly: true
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    font.pointSize: Style.fontSize.normal
}
