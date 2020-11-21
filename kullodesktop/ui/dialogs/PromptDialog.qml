/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo.Visuals 1.0

import "../"
import "../native"

BaseDialog {
    id: root

    property alias text: contentText.text
    property alias result: contentInput.text
    property var validator: function(x) { return x.trim() !== ""; }
    property bool valid: validator(result)

    property bool errorHintVisible: false
    property alias errorHintText: errorHintTextBox.text

    width: 400 * Hdpi.FontScalingFactor
    height: mainItem.implicitHeight
            + 2*verticalPadding
    title: text

    Component.onCompleted: mainItem.forceActiveFocus()

    signal resultAccepted()


    // Main Qt Quick Item required for attached property `Keys`
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
        implicitHeight: header.implicitHeight + body.implicitHeight + footer.implicitHeight

        Item {
            id: header
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            implicitHeight: contentText.implicitHeight

            NativeText {
                id: contentText
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                text: ""
            }
        }

        Item {
            id: body
            anchors {
                top: header.bottom
                left: parent.left
                right: parent.right
            }

            implicitHeight: contentInput.implicitHeight + errorHint.height + contentInput.anchors.topMargin

            TextField {
                id: contentInput
                anchors {
                    top: parent.top
                    topMargin: 5 * Hdpi.FontScalingFactor
                    left: parent.left
                    right: parent.right
                }
                focus: true
                text: ""
                inputMethodHints: Qt.ImhNoAutoUppercase

                onAccepted: root.accepted()
            }

            Hint {
                id: errorHint
                anchors {
                    left: parent.left
                    leftMargin: 150 * Hdpi.FontScalingFactor
                    right: parent.right
                    rightMargin: 20 * Hdpi.FontScalingFactor
                    bottom: parent.bottom
                }
                height: errorHintVisible ? 30 * Hdpi.FontScalingFactor : 0
                paddingTop: 10 * Hdpi.FontScalingFactor
                color: Style.red
                clip: true

                NativeText {
                    id: errorHintTextBox
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 2 * Hdpi.FontScalingFactor
                        left: parent.left
                        leftMargin: 10 * Hdpi.FontScalingFactor
                    }
                    color: "white"
                }
            }
        }

        Item {
            id: footer
            anchors {
                top: body.bottom
                left: parent.left
                right: parent.right
            }
            implicitHeight: footerRow.implicitHeight + footerRow.anchors.topMargin

            Row {
                id: footerRow
                anchors {
                    top: parent.top
                    topMargin: contentFooterSpacing
                    right: parent.right
                }
                spacing: buttonsSpacing

                NativeButton {
                    id: buttonCancel
                    text: qsTr("Cancel")

                    onClicked: root.rejected()
                }

                NativeButton {
                    id: buttonOk
                    text: qsTr("OK")
                    enabled: valid

                    onClicked: root.accepted()
                }
            }
        }
    }

    onAccepted: {
        if (valid)
        {
            root.resultAccepted()
            root.closeDialog()
        }
    }
    onRejected: root.closeDialog()
}
