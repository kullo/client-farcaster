/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.4
import Kullo 1.0

import "../"
import "../buttons"
import "../dialogs"
import "../formelements"
import "../misc"
import "../native"

FForm {
    id: root
    property var registerer: null

    signal done()

    anchors {
        fill: parent
    }

    Component.onCompleted: {
        reset()
    }

    function reset() {
        domainSelection.domain = "kullo.net"
    }

    validator: function() {
        return !registerer.locked
    }

    Connections {
        target: registerer

        onInvalidKulloAddress: {
            registerer.locked = false
            console.warn("Invalid Kullo address format: '" + address +  "'");
            errorHintUser.text = qsTr("Invalid Kullo address format.")
        }
        onAddressExists: {
            registerer.locked = false
            console.warn("Kullo address already exists: '" + address +  "'");
            errorHintUser.text = qsTr("Kullo address already exists.")
        }
        onAddressBlocked: {
            registerer.locked = false
            console.warn("Kullo address blocked: '" + address +  "'");
            errorHintUser.text = qsTr("Kullo address is blocked.")
        }
        onChallengeNeeded: {
            registerer.locked = false
            blockChallenge.type = type
            blockChallenge.address = address
            blockChallenge.text = (type === ChallengeTypes.Reservation
                                   ? qsTr("Please enter the reservation code for address '%1'.").arg(address)
                                   : (type === ChallengeTypes.Code
                                      ? qsTr("Please enter an invite code.")
                                      : (type === ChallengeTypes.Reset
                                         ? qsTr("Please enter the reset code for address '%1'.").arg(address)
                                         : ""
                                         )
                                      )
                                   )
            blockChallenge.visible = true

            blockChallenge.forceActiveFocus()
        }
        onSucceeded: {
            registerer.locked = false
            console.info("Registration succeeded for address: '" + address + "'" )
            Inbox.loadCredentials(address)
            root.done()
        }
        onServerError: {
            registerer.locked = false
            errorHintGlobal.text = qsTr("Server error, please try again later.")
        }
        onProtocolError: {
            registerer.locked = false
            errorHintGlobal.text = qsTr("Protocol error. This is our fault, sorry!")
        }
        onNetworkError: {
            registerer.locked = false
            errorHintGlobal.text = qsTr("Network error. Are you online?")
        }
    }

    onAccepted: {
        var registrationChallengeAnswer = challengeAnswer.text
        var registrationKulloAddress = usernameInput.text + "#" + domainSelection.domain
        blockChallenge.reset()
        errorHintGlobal.text = ""
        errorHintUser.text = ""
        registerer.locked = true
        registerer.registerAccount(registrationKulloAddress, registrationChallengeAnswer)
    }

    Column {
        id: column1
        anchors.fill: parent
        spacing: 30 * Hdpi.FontScalingFactor

        BackButton {
            onClicked: app.state = "welcome"
        }

        Column {
            id: blockAddress
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 8 * Hdpi.FontScalingFactor

            NativeText {
                text: qsTr("Choose your Kullo address:")
            }

            Item {
                id: blockAddressInput
                anchors {
                    left: parent.left
                    right: parent.right
                }
                implicitHeight: usernameInput.implicitHeight

                FTextField {
                    id: usernameInput
                    focus: true
                    anchors {
                        left: parent.left
                        right: domainSelection.left
                    }

                    inputMethodHints: Qt.ImhNoAutoUppercase
                    font.pointSize: Style.fontSize.registrationKulloAdressUsername
                    onTextChanged: {
                        errorHintUser.text = ""
                        blockChallenge.reset()
                        if (text.substr(-10) === "#kullo.net")
                        {
                            text = text.substr(0, text.length-10);
                        }
                    }
                    placeholderText: qsTr("username")
                    validator: RegExpValidator {
                        regExp: /^[a-zA-Z0-9]+([-._][a-zA-Z0-9]+)*$/
                    }
                    maximumLength: 64
                }

                Row {
                    id: domainSelection
                    property string domain

                    anchors.right: parent.right

                    NativeText {
                        text: "#"
                        font.pointSize: domainText.font.pointSize
                        height: usernameInput.implicitHeight
                        verticalAlignment: Text.AlignVCenter
                    }

                    NativeText {
                        id: domainText
                        text: domainSelection.domain
                        font.pointSize: Style.fontSize.registrationKulloAdressDomain
                        font.underline: toolTip.containsMouse
                        height: usernameInput.implicitHeight
                        verticalAlignment: Text.AlignVCenter

                        TooltipArea {
                            id: toolTip
                            text: qsTr("Select custom domain")
                        }

                        PromptDialog {
                            id: customDomainDialog
                            objectName: "customDomainDialog"
                            title: toolTip.text
                            text: qsTr("Enter a custom domain if the Kullo staff instructed you to do so.")
                                  + " "
                                  + qsTr("Otherwise cancel this dialog and use 'kullo.net'.")

                            validator: function(x) { return Utils.isValidKulloDomain(x) }

                            onResultAccepted: {
                                domainSelection.domain = result
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: customDomainDialog.openDialog()
                        }
                    }
                }
            }

            ErrorHint {
                id: errorHintUser
                anchors {
                    right: parent.right
                    rightMargin: 20 * Hdpi.FontScalingFactor
                }
                maxWidth: blockAddress.width - anchors.rightMargin
                direction: "topleft"
            }
        }

        FocusScope {
            id: blockChallenge
            property int type: ChallengeTypes.None
            property string address
            property alias text: challengeText.text

            function reset() {
                blockChallenge.visible = false
                blockChallenge.address = ""
                blockChallenge.text = ""

                challengeAnswer.text = ""
            }

            anchors {
                left: parent.left
                right: parent.right
            }
            visible: false
            implicitHeight: challengeBlockContent.implicitHeight

            Column {
                id: challengeBlockContent
                spacing: 8 * Hdpi.FontScalingFactor
                anchors {
                    left: parent.left
                    right: parent.right
                }

                NativeText {
                    id: challengeText
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                }

                FTextField {
                    id: challengeAnswer
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    focus: true
                    font.pointSize: Style.fontSize.registrationChallengeAnswerInput
                    placeholderText: (blockChallenge.type === ChallengeTypes.Reservation
                                      ? qsTr("Reservation code")
                                      : (blockChallenge.type === ChallengeTypes.Code
                                         ? qsTr("Invite code")
                                         : (blockChallenge.type === ChallengeTypes.Reset
                                            ? qsTr("Reset code")
                                            : ""
                                            )
                                         )
                                      )
                }
            }
        }

        Item {
            id: blockTerms
            anchors {
                left: parent.left
                right: parent.right
            }
            implicitHeight: Math.max(termsAccepted.implicitHeight, termsText.implicitHeight)

            CheckBox {
                id: termsAccepted
                anchors {
                    left: parent.left
                }
            }

            NativeTextWithLinks {
                id: termsText
                anchors {
                    left: termsAccepted.right
                    leftMargin: 5 * Hdpi.FontScalingFactor
                    right: parent.right
                }
                text: qsTr("I accept the <a href='%1'>terms of service</a>").arg(registerer.termsUrl)
            }
        }

        Column {
            id: blockRegisterButtom
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 10 * Hdpi.FontScalingFactor

            Row {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                layoutDirection: Qt.RightToLeft

                FSubmitButton {
                    text: qsTr("Register")
                    enabled: usernameInput.text !== ""
                             && termsAccepted.checked == true
                             && !registerer.locked
                    style: KulloButtonStyle {}
                }
            }

            ErrorHint {
                id: errorHintGlobal
                anchors {
                    left: parent.left
                    leftMargin: 15 * Hdpi.FontScalingFactor
                }
                maxWidth: blockRegisterButtom.width - anchors.leftMargin
            }
        }
    }
}
