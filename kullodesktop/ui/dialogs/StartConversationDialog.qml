/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0
import Kullo.Visuals 1.0

import "../"
import "../misc"
import "../native"
import "../js/shortcut.js" as SC

BaseDialog {
    objectName: "StartConversationDialog"

    /* public */
    property alias inputText: input.editText
    property string result

    /* private */
    property var _addresses
    property alias _errorHintText: errorHint.text
    property bool _errorHintVisible: _errorHintText !== ""
    property bool _stateClosing: false

    signal addressAccepted()
    signal addressAdded()

    id: root
    width: 400 * Hdpi.FontScalingFactor
    minimumHeight: mainItem.implicitHeight + 2*verticalPadding
    height: mainItem.implicitHeight + 2*verticalPadding

    Component.onCompleted: {
        mainItem.forceActiveFocus()
    }

    function reset() {
        // Reset model first to ensure text input is cleared afterwards
        input.reloadAutocompletionModel()

        _addresses = []
        result = ""
        input.editText = ""
        for(var i = stagedAddresses.children.length; i > 1; i--)
        {
            stagedAddresses.children[i-1].destroy()
        }
    }

    function addAddresses(addressList) {
        for (var i = 0; i < addressList.length; ++i) {
            var address = addressList[i]
            addAddress(address)
        }
    }

    function addAddress(address)
    {
        _addresses.push(address)
        stagedAddresses.add(address)
        root.addressAdded()
        input.editText = ""
    }

    // on open window
    onVisibleChanged: {
        if (visible) {
            reset()
        }
    }

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

        implicitHeight: body.implicitHeight + footer.implicitHeight

        Item {
            id: body
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            implicitHeight: stagedAddresses.implicitHeight
                            + inputRowBox.anchors.topMargin
                            + inputRowBox.implicitHeight
                            + errorHint.height

            Flow {
                id: stagedAddresses
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                spacing: 5 * Hdpi.FontScalingFactor

                NativeText {
                    id: stagedAddressesLabel
                    text: qsTr("Recipients:")
                }

                Component {
                    id: stagedAddress

                    NativeText {
                        font.family: FontList.MonospaceFamily
                        height: stagedAddressesLabel.implicitHeight
                        verticalAlignment: Text.AlignBottom
                    }
                }

                function add(address) {
                    var obj = stagedAddress.createObject(stagedAddresses, {"text": address});
                    if (!obj) console.error("Error creating object");
                }
            }

            Item {
                id: inputRowBox

                anchors {
                    top: stagedAddresses.bottom
                    topMargin: 5 * Hdpi.FontScalingFactor
                    left: parent.left
                    right: parent.right
                }
                implicitHeight: input.implicitHeight
                height: implicitHeight

                ComboBoxKulloAddress {
                    id: input

                    anchors {
                        top: parent.top
                        left: parent.left
                        right: addButton.left
                        rightMargin: 2 * Hdpi.FontScalingFactor
                    }

                    focus: true
                    editText: ""
                    enabled: !existenceChecker.locked

                    onEditTextChanged: _errorHintText = ""

                    ExistenceChecker {
                        id: existenceChecker
                        client: Inbox.client()

                        onExistenceChecked: {
                            if (ok) {
                                console.info("Existence ok: '" + address + "'")
                                _errorHintText = ""
                                root.addAddress(address)

                                if (root._stateClosing)
                                {
                                    root.accepted()
                                }
                            }
                            else {
                                console.info("Existence failed: " + address)
                                _errorHintText = qsTr("Address does not exist.")
                            }
                            root._stateClosing = false
                        }
                        onNetworkError: {
                            console.info("Start: Existence failed: " + address)
                            _errorHintText = qsTr("Address could not be verified. Are you online? Is the address correct?")
                        }
                    }

                    function tryToAddValueToList()
                    {
                        var address = editText.trim().toLowerCase()

                        if (!Utils.isValidKulloAddress(address))
                        {
                            return false
                        }
                        if (Utils.kulloAddressEqual(address, Inbox.userSettings.address))
                        {
                            _errorHintText = qsTr("Monologues not supported.")
                            return false
                        }
                        if (_addresses.indexOf(address) !== -1)
                        {
                            _errorHintText = qsTr("Address already added.")
                            return false
                        }

                        existenceChecker.checkExistence(address)

                        return true
                    }

                    onAccepted: tryToAddValueToList()

                    Keys.onPressed: {
                        if (SC.isCtrlAndKey(Qt.Key_Return, event))
                        {
                            event.accepted = true
                            root.accepted()
                        }
                    }
                }

                NativeButton {
                    id: addButton
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    width: implicitHeight
                    text: "+"
                    onClicked: input.tryToAddValueToList()
                }
            }

            ErrorHint {
                id: errorHint
                anchors {
                    right: parent.right
                    rightMargin: 20 * Hdpi.FontScalingFactor
                    bottom: parent.bottom
                }
                maxWidth: parent.width-2*anchors.rightMargin
                direction: "topleft"
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
                    id: buttonOk
                    text: qsTr("OK")
                    enabled: true

                    onClicked: root.accepted()
                }

                NativeButton {
                    id: buttonCancel
                    text: qsTr("Cancel")

                    onClicked: root.rejected()
                }
            }
        }
    }

    on_ErrorHintVisibleChanged: {
        buttonOk.enabled = !_errorHintVisible
    }

    onAccepted: {
        // Something in input?
        if (input.editText.trim() != "")
        {
            if (!input.tryToAddValueToList())
            {
                return
            }
            else
            {
                // Wait for existence checker ...
                root._stateClosing = true
                return
            }
        }

        // Called from OK button click or Ctrl+Enter
        // Double check input here
        if (_addresses.length)
        {
            result = ""
            for (var i = 0; i < _addresses.length; ++i)
            {
                if (i) result += ","
                result += _addresses[i]
            }
            root.addressAccepted()
            root.closeDialog()
        }
    }

    onRejected: {
        root.closeDialog()
    }
}
