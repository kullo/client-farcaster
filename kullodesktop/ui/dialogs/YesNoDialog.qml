/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../native"

BaseDialog {
    id: _root
    property alias text: mainText.text

    signal yes()
    signal no()

    Component.onCompleted: content.forceActiveFocus()

    minimumHeight: mainColumn.implicitHeight + 2*verticalPadding
    height: mainColumn.implicitHeight + 2*verticalPadding

    Item {
        id: mainItem
        anchors.fill: parent

        Keys.onEscapePressed: _root.rejected()

        FocusScope {
            id: content
            anchors {
                fill: parent
                topMargin: verticalPadding
                bottomMargin: verticalPadding
                leftMargin: horizontalPadding
                rightMargin: horizontalPadding
            }

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

                    Keys.onReturnPressed: {
                        if (buttonYes.focus) {
                            buttonYes.clicked()
                        }
                        if (buttonNo.focus) {
                            buttonNo.clicked()
                        }
                    }

                    NativeButton {
                        id: buttonNo
                        text: qsTr("No")
                        onClicked: {
                            _root.closeDialog()
                            _root.no()
                        }
                        isDefault: focus
                    }

                    NativeButton {
                        id: buttonYes
                        text: qsTr("Yes")
                        onClicked: {
                            _root.closeDialog()
                            _root.yes()
                        }
                        focus: true
                        isDefault: focus
                    }
                }
            }
        }
    }
}
