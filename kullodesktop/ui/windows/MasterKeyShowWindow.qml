/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../"
import "../buttons"
import "../misc"
import "../native"

NativeWindow {
    /* public */
    property string address
    property alias key: keyInput.text

    /* private */
    property string _descriptionTextPrexfix: qsTr("Kullo MasterKey of")
    property string _descriptionText: _descriptionTextPrexfix + " " + address
    id: root

    title: "MasterKey"
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

        Keys.onEscapePressed: root.closeWindow()
        Keys.onPressed: {
            handleNativeWindowShortcuts(event)

            // CTRL + P
            if ((event.modifiers & Qt.ControlModifier) && event.key === Qt.Key_P) {
                console.info("MasterKeyShowWindow: Ctrl+P pressed.")
                Inbox.userSettings.printMasterKey(_descriptionText)
                event.accepted = true
            }
        }

        Item {
            id: windowContent
            anchors {
                fill: parent
            }
            property int spacing: 10
            implicitHeight: infoTextRow.implicitHeight
                            + keyInput.implicitHeight
                            + printButton.implicitHeight
                            + 2*spacing

            Row {
                id: infoTextRow
                NativeText {
                    text: _descriptionTextPrexfix + " "
                }
                NativeSelectableText {
                    id: addressElement
                    text: address

                    CutCopyPasteMenu {
                        hasCopy: true
                        hasSelectAll: true
                        hasPaste: false
                        hasCut: false

                        onSelectAll: addressElement.selectAll()
                        onCopy: addressElement.copy()
                    }
                }
            }

            TextArea {
                id: keyInput
                anchors {
                    left: parent.left
                    right: parent.right
                    top: infoTextRow.bottom
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
                    Inbox.userSettings.printMasterKey(_descriptionText)
                }
            }
        }
    }
}
