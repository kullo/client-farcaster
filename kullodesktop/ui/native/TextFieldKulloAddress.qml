/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3

TextField {
    id: _root

    property bool isValid: false
    property int _lastResultLength: 0

    inputMethodHints: Qt.ImhNoAutoUppercase
    placeholderText: qsTr("Kullo address")

    onTextChanged: {
        // Manipulate input
        if (text.trim() != text) {
            text = text.trim()
        }

        if (text.length == _lastResultLength+1
                && text.charAt(text.length-1) == "@") {
            text = text.substring(0, text.length-1) + "#"
        }

        _lastResultLength = text.length

        // Validate input
        isValid = Utils.isValidKulloAddress(text)
    }
}
