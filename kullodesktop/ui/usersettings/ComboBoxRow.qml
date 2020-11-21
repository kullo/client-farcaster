/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.2

import "../native"

Item {
    id: _root
    property alias name: label.text
    property var values: ListModel {}
    property var value
    property var initialValue
    property string description: ""

    property int marginTop: 0
    property int marginBottom: 0

    onValueChanged: {
        // When parent changes value property
        var findResult = -1
        for (var pos = 0; pos < combobox.model.count; ++pos)
        {
            if (combobox.model.get(pos).value === value) findResult = pos
        }

        if (findResult !== -1)
        {
            combobox.currentIndex = findResult
        }
        else
        {
            console.warn("Value not found in ComboBox model: '" + value + "'")
        }
    }

    Component.onCompleted: {
        var initialValue = _root.initialValue

        var findResult = -1
        for (var pos = 0; pos < combobox.model.count; ++pos)
        {
            if (combobox.model.get(pos).value === initialValue) findResult = pos
        }

        if (findResult !== -1)
        {
            combobox.currentIndex = findResult
        }
        else
        {
            console.warn("Initial value not found in ComboBox model: '" + initialValue + "'")
        }
    }

    anchors {
        left: parent.left
        right: parent.right
    }

    height: implicitHeight
    implicitHeight: marginTop
                    + combobox.height
                    + (_description.visible ? _description.anchors.topMargin + _description.implicitHeight : 0)
                    + marginBottom

    RowLabel {
        id: label
        height: combobox.height
    }

    ComboBox {
        id: combobox

        anchors {
            left: label.right
            right: parent.right
        }
        y: marginTop

        model: _root.values

        onCurrentIndexChanged: {
            console.debug("Selected: " + model.get(currentIndex).text + ", " + model.get(currentIndex).value)
            var value = model.get(currentIndex).value
            _root.value = value
        }

    }

    NativeText {
        id: _description
        anchors {
            left: label.right
            right: parent.right
            top: combobox.bottom
            topMargin: 4 * Hdpi.FontScalingFactor
        }
        text: _root.description
        visible: text !== ""
    }
}
