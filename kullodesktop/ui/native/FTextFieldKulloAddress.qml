/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../formelements"

FTextField {
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
