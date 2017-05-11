/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../native"

BaseDialog {
    id: root
    property alias text: mainText.text

    signal yes()
    signal no()

    onAccepted: {
        closeDialog()
        yes()
    }

    onRejected: {
        closeDialog()
        no()
    }

    Component.onCompleted: mainItem.forceActiveFocus()

    minimumHeight: mainColumn.implicitHeight + 2*verticalPadding
    height: mainColumn.implicitHeight + 2*verticalPadding

    FocusScope {
        id: mainItem
        anchors {
            fill: parent
            topMargin: verticalPadding
            bottomMargin: verticalPadding
            leftMargin: horizontalPadding
            rightMargin: horizontalPadding
        }

        Keys.onEscapePressed: root.rejected()

        Column {
            id: mainColumn
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: contentFooterSpacing

            NativeText {
                id: mainText
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }

            Row {
                id: buttonsRow
                anchors {
                    left: parent.left
                    right: parent.right
                }
                layoutDirection: Qt.RightToLeft
                spacing: buttonsSpacing

                NativeButton {
                    id: buttonNo
                    text: qsTr("No")
                    onClicked: {
                        root.closeDialog()
                        root.no()
                    }
                    isDefault: focus
                }

                NativeButton {
                    id: buttonYes
                    text: qsTr("Yes")
                    onClicked: {
                        root.closeDialog()
                        root.yes()
                    }
                    focus: true
                    isDefault: focus
                }
            }
        }
    }
}
