/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../"
import "../buttons"
import "../misc"
import "../native"
import "../js/format.js" as Format

NativeWindow {
    id: root

    title: qsTr("Import MasterKey")
    width: 400 * Hdpi.FontScalingFactor
    height: windowContent.anchors.topMargin +
            windowContent.implicitHeight +
            windowContent.anchors.bottomMargin

    onVisibleChanged: if (visible) keyInput.forceActiveFocus()

    Rectangle {
        id: mainItem
        anchors {
            fill: parent
        }
        color: Style.beigeLight

        Keys.onEscapePressed: root.closeWindow()
        Keys.onPressed: handleNativeWindowShortcuts(event)

        Column {
            id: windowContent
            anchors {
                fill: parent
                margins: 10 * Hdpi.FontScalingFactor
            }
            spacing: 10 * Hdpi.FontScalingFactor

            NativeText {
                text: qsTr("Import Kullo MasterKey")
            }

            TextArea {
                id: keyInput
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: 150 * Hdpi.FontScalingFactor
                focus: true

                textFormat: TextEdit.PlainText
                font: FontList.Monospace

                text: ""

                CutCopyPasteMenu {
                    onCopy: keyInput.copy()
                    onPaste: keyInput.paste()
                }
            }

            Row {
                anchors.left: parent.left
                anchors.right: parent.right
                layoutDirection: Qt.RightToLeft

                NativeButton {
                    style: KulloButtonStyle {}
                    text: qsTr("Import")
                    onClicked: {
                        keyData.clearInputs()
                        if (Utils.isValidPemMasterKey(keyInput.text))
                        {
                            var keyParts = Utils.extractDataFromPemMasterKey(keyInput.text)
                            for (var i = 0; i < keyParts.length; i++) {
                                var element = keyData.findChildByName("key" + Format.pad_left(i+1, 2))
                                element.text = keyParts[i]
                            }
                        }
                        root.closeWindow()
                    }
                }
            }
        }
    }
}
