/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../"
import "../buttons"
import "../misc"
import "../native"
import "../js/format.js" as Format

NativeWindow {
    id: _root

    property string address
    property alias key: keyInput.text
    property string _intoText: qsTr("Kullo MasterKey of %1").arg(address)

    title: qsTr("Show MasterKey")
    width: 400
    height: mainItem.anchors.topMargin +
            windowContent.implicitHeight +
            mainItem.anchors.bottomMargin

    onVisibleChanged: if (visible) keyInput.forceActiveFocus()

    FocusScope {
        id: mainItem
        anchors {
            fill: parent
            margins: 10
        }

        Keys.onEscapePressed: _root.closeWindow()
        Keys.onPressed: {
            handleCtrlW(event)

            // CTRL + P
            if ((event.modifiers & Qt.ControlModifier) && event.key === Qt.Key_P) {
                console.info("MasterKeyShowWindow: Ctrl+P pressed.")
                Client.userSettings.printMasterKey(_intoText)
                event.accepted = true
            }
        }

        Item {
            id: windowContent
            anchors {
                fill: parent
            }
            property int spacing: 10
            implicitHeight: intoText.implicitHeight
                            + keyInput.implicitHeight
                            + printButton.implicitHeight
                            + 2*spacing

            NativeText {
                id: intoText
                text: _intoText
            }

            TextArea {
                id: keyInput
                anchors {
                    left: parent.left
                    right: parent.right
                    top: intoText.bottom
                    bottom: printButton.top
                    topMargin: windowContent.spacing
                    bottomMargin: windowContent.spacing
                }

                textFormat: TextEdit.PlainText
                font: FontList.Monospace
                readOnly: true
                focus: true

                text: ""

                CutCopyPasteMenu {
                    hasCut: false
                    hasPaste: false
                    hasSelectAll: true
                    onCopy: keyInput.copy()
                    onSelectAll: keyInput.selectAll()
                }
            }

            NativeButton {
                id: printButton
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }
                text: qsTr("Print")
                onClicked: {
                    console.info("MasterKeyShowWindow: Print button clicked.")
                    Client.userSettings.printMasterKey(_intoText)
                }
            }
        }
    }
}
