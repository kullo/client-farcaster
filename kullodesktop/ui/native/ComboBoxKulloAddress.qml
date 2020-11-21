/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3

ComboBox {
    property bool isValid: false

    /* private */
    id: root
    property int _lastResultLength: 0

    editable: true
    inputMethodHints: Qt.ImhNoAutoUppercase
    //placeholderText: qsTr("Kullo address")

    // Addresses are not loaded automatically
    function reloadAutocompletionModel() {
        model = Inbox.allKnownUsersSorted()
    }

    onEditTextChanged: {
        // Manipulate input
        if (editText.trim() != editText) {
            editText = editText.trim()
        }

        if (editText.length == _lastResultLength+1
                && editText.charAt(editText.length-1) == "@") {
            editText = editText.substring(0, editText.length-1) + "#"
        }

        _lastResultLength = editText.length

        // Validate input
        isValid = Utils.isValidKulloAddress(editText)
    }
}
