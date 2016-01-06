/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0
import Kullo.Visuals 1.0

import "../"
import "../misc"
import "../native"
import "../js/js_helpers.js" as JSHelpers
import "../js/shortcut.js" as SC

BaseDialog {
    id: _root
    objectName: "GroupConversationChangeDialog"

    property var initialParticipants: [] // string list
    property string text
    property alias buttonOkEnabled: buttonOk.enabled
    property alias buttonCancelEnabled: buttonCancel.enabled
    property alias errorHintText: errorHint.text
    property bool errorHintVisible: errorHintText != ""
    property var addresses: [] // string list
    property string result
    property bool state_closing: false

    signal addressAccepted()
    signal addressAdded()
    signal addressRemoved()

    width: 400
    minimumHeight: mainItem.implicitHeight + 2*verticalPadding
    height: mainItem.implicitHeight + 2*verticalPadding
    title: text

    Component.onCompleted: {
        mainItem.forceActiveFocus()
    }

    function buttonOkEnablesValidator() {
        if (addresses.length)
            return true
        else
            return false
    }

    function reset() {
        addresses = initialParticipants
        result = ""
        input.text = ""
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

            implicitHeight: stagedAddressesBox.height
                            + inputRowBox.anchors.topMargin
                            + inputRowBox.implicitHeight
                            + errorHint.height


            NativeText {
                id: stagedAddressesLabel
                text: qsTr("Recipients:")
            }

            ScrollView {
                id: stagedAddressesBox
                anchors {
                    top: parent.top
                    left: stagedAddressesLabel.right
                    leftMargin: 20
                    right: parent.right
                }
                height: 100

                ListView {
                    id: stagedAddresses
                    anchors.fill: parent
                    clip: true
                    interactive: false
                    model: addresses

                    function removeValueFromList(address)
                    {
                        var copy = addresses.slice(addresses, addresses.length)
                        JSHelpers.removeFromArray(copy, address)
                        addresses = copy
                        _root.addressRemoved()
                    }

                    delegate: Item {
                        height: 32

                        property string address: modelData

                        Image {
                            id: avatar
                            source: "image://participantavatars/" + Utils.urlencode(address)
                            property int size: 30
                            width: size
                            height: size
                            sourceSize.width: size
                            sourceSize.height: size

                            anchors {
                                left: parent.left
                                top: parent.top
                                topMargin: 1
                            }
                        }

                        NativeText {
                            id: label
                            anchors {
                                left: avatar.right
                                leftMargin: 8
                                top: parent.top
                                topMargin: 1
                            }
                            text: address
                            height: avatar.height
                            verticalAlignment: Text.AlignVCenter
                        }

                        NativeImage {
                            source: "/resources/scalable/minus_b.svg"
                            width: 14
                            height: 14
                            anchors {
                                top: parent.top
                                topMargin: (parent.height-height) / 2
                                left: label.right
                                leftMargin: 5
                            }

                            TooltipArea {
                                text: qsTr("Remove this recipient")
                                onClicked: {
                                    stagedAddresses.removeValueFromList(address)
                                }
                            }
                        }
                    }
                }
            }

            Item {
                id: inputRowBox

                anchors {
                    top: stagedAddressesBox.bottom
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
                        client: ClientNewApi

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
                                console.info("Existence failed: '" + address + "'")
                                errorHintText = qsTr("Address does not exist.")
                            }
                            _root.state_closing = false
                        }
                        onNetworkError: {
                            console.info("Existence failed: '" + address + "'")
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
                        var copy = addresses.slice(addresses, addresses.length)
                        copy.push(address)

                        addresses = copy

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
                    enabled: buttonOkEnablesValidator()

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
