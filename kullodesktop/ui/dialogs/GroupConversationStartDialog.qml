/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0
import Kullo.Visuals 1.0

import "../"
import "../misc"
import "../native"
import "../js/shortcut.js" as SC

BaseDialog {
    id: _root
    objectName: "GroupConversationStartDialog"

    property string text
    property alias buttonOkEnabled: buttonOk.enabled
    property alias buttonCancelEnabled: buttonCancel.enabled
    property alias errorHintText: errorHint.text
    property bool errorHintVisible: errorHintText !== ""
    property var addresses
    property string result
    property bool state_closing: false

    signal addressAccepted()
    signal addressAdded()

    width: 400
    minimumHeight: mainItem.implicitHeight + 2*verticalPadding
    height: mainItem.implicitHeight + 2*verticalPadding
    title: text

    Component.onCompleted: {
        mainItem.forceActiveFocus()
    }

    function reset() {
        addresses = []
        result = ""
        input.text = ""
        for(var i = stagedAddresses.children.length; i > 1; i--)
        {
            stagedAddresses.children[i-1].destroy()
        }
    }

    // on open window
    onVisibleChanged: if (visible) reset()

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
                spacing: 5

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
                    topMargin: 5
                    left: parent.left
                    right: parent.right
                }
                implicitHeight: input.implicitHeight
                height: implicitHeight

                TextFieldKulloAddress {
                    id: input

                    anchors {
                        top: parent.top
                        left: parent.left
                        right: addButton.left
                        rightMargin: 2
                    }
                    focus: true
                    text: ""
                    enabled: !existenceChecker.locked

                    onTextChanged: errorHintText = ""

                    ExistenceChecker {
                        id: existenceChecker
                        client: Client.client()

                        onExistenceChecked: {
                            if (ok) {
                                console.info("Existence ok: '" + address + "'")
                                errorHintText = ""
                                input.addValueToList(address)

                                if (_root.state_closing)
                                {
                                    _root.accepted()
                                }
                            }
                            else {
                                console.info("Existence failed: " + address)
                                errorHintText = qsTr("Address does not exist.")
                            }
                            _root.state_closing = false
                        }
                        onNetworkError: {
                            console.info("Start: Existence failed: " + address)
                            errorHintText = qsTr("Address could not be verified. Are you online? Is the address correct?")
                        }
                    }

                    function tryToAddValueToList()
                    {
                        var address = text.trim().toLowerCase()

                        if (!Utils.isValidKulloAddress(address))
                        {
                            return false
                        }
                        if (Utils.kulloAddressEqual(address, Client.userSettings.address))
                        {
                            errorHintText = qsTr("Monologues not supported.")
                            return false
                        }
                        if (addresses.indexOf(address) !== -1)
                        {
                            errorHintText = qsTr("Address already added.")
                            return false
                        }

                        existenceChecker.checkExistence(address)

                        return true
                    }

                    function addValueToList(address)
                    {
                        addresses.push(address)
                        stagedAddresses.add(address)
                        _root.addressAdded()
                        input.text = ""
                    }

                    onAccepted: tryToAddValueToList()

                    Keys.onPressed: {
                        if (SC.isCtrlAndKey(Qt.Key_Return, event))
                        {
                            event.accepted = true
                            _root.accepted()
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
                    rightMargin: 20
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

                    onClicked: _root.accepted()
                }

                NativeButton {
                    id: buttonCancel
                    text: qsTr("Cancel")

                    onClicked: _root.rejected()
                }
            }
        }
    }

    onErrorHintVisibleChanged: buttonOkEnabled = !errorHintVisible

    onAccepted: {
        // Something in input?
        if (input.text.trim() != "")
        {
            if (!input.tryToAddValueToList())
            {
                return
            }
            else
            {
                // Wait for existence checker ...
                _root.state_closing = true
                return
            }
        }

        // Called from OK button click or Ctrl+Enter
        // Double check input here
        if (addresses.length)
        {
            result = ""
            for (var i = 0; i < addresses.length; ++i)
            {
                if (i) result += ","
                result += addresses[i]
            }
            _root.addressAccepted()
            _root.closeDialog()
        }
    }

    onRejected: {
        _root.closeDialog()
    }
}
