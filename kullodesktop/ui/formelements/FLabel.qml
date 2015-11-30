/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

Label {
    property var target: null
    property bool _FORM_TYPE_IGNORE_IN_FORM_VALIDATION: true
    property bool _FORM_TYPE_IGNORE_IN_FORM_CLEARING: true

    width: 100
    height: target ? target.implicitHeight : 62
    verticalAlignment: Text.AlignVCenter

    MouseArea {
        anchors.fill: parent
        onClicked: target.forceActiveFocus()
    }
}
