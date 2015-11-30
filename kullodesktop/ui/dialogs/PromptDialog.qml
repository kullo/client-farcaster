/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo.Visuals 1.0

import "../"
import "../native"

BaseDialog {
    id: _root

    property alias text: contentText.text
    property alias result: contentInput.text
    property var validator: function(x) { return x.trim() !== ""; }
    property bool valid: validator(result)

    property bool errorHintVisible: false
    property alias errorHintText: errorHintTextBox.text

    width: 400
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
        Keys.onEscapePressed: _root.rejected()
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
                    topMargin: 5
                    left: parent.left
                    right: parent.right
                }
                focus: true
                text: ""
                inputMethodHints: Qt.ImhNoAutoUppercase

                onAccepted: _root.accepted()
            }

            Hint {
                id: errorHint
                anchors {
                    left: parent.left
                    leftMargin: 150
                    right: parent.right
                    rightMargin: 20
                    bottom: parent.bottom
                }
                height: errorHintVisible ? 30 : 0
                paddingTop: 10
                color: Style.red
                clip: true

                NativeText {
                    id: errorHintTextBox
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 2
                        left: parent.left
                        leftMargin: 10
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

                    onClicked: _root.rejected()
                }

                NativeButton {
                    id: buttonOk
                    text: qsTr("OK")
                    enabled: valid

                    onClicked: _root.accepted()
                }
            }
        }
    }

    onAccepted: {
        if (valid)
        {
            _root.resultAccepted()
            _root.closeDialog()
        }
    }
    onRejected: _root.closeDialog()
}
