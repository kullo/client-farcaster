/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Window 2.2
import Kullo 1.0

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "buttons"
import "formelements"
import "misc"
import "native"
import "windows"
import "js/format.js" as Format

FForm {
    id: root
    anchors.fill: parent

    onSubmitted: {
        if (!loginChecker.locked)
        {
            loginFailedErrorHint.text = ""
            loginChecker.run(userAdress.text, keyData.getKey())
        }
        else
        {
            console.debug("Tried to submit but LoginChecker is locked.")
        }
    }

    MasterKeyImportWindow {
        id: importWindow
    }

    Rectangle {
        anchors.fill: parent
        color: Style.beigeLight

        Logo {
            id: logo
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 10
                rightMargin: anchors.topMargin
            }
        }

        Column {
            id: column1
            width: 260
            anchors {
                top: logo.bottom
                topMargin: 50
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                leftMargin: 20
                rightMargin: 20
            }
            spacing: 15

            BackButton {
                onClicked: app.state = "welcome"
            }

            FTextFieldKulloAddress {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                id: userAdress
                font.pointSize: Style.fontSize.loginKulloAddressInput
            }

            Row {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                layoutDirection: Qt.RightToLeft

                NativeText {
                    text: qsTr("Import")
                    font.underline: true

                    MouseArea {
                        anchors.fill: parent
                        onClicked: importWindow.openWindow()
                    }
                }
            }

            Flow {
                id: keyData
                anchors {
                    left: parent.left
                    right: parent.right
                }

                spacing: 5

                function setMasterKey(keyParts) {
                    keyData.clearInputs()
                    for (var i = 0; i < keyParts.length; i++) {
                        var element = keyData.findChildByName("key" + Format.pad_left(i+1, 2))
                        element.text = keyParts[i]
                    }
                }

                function findChildByName(objectName) {
                    for (var i = 0; i < this.children.length; i++) {
                        if(this.children[i].objectName === objectName) return this.children[i];
                    }
                    console.warn("Child width objectName '" + objectName + "' not found.")
                }

                function clearInputs() {
                    for (var i = 0; i < this.children.length; i++) {
                        if (this.children[i].objectName.match(/^key/)) this.children[i].text = "";
                    }
                }

                function getKey() {
                    var blocksCount = this.children.length
                    var output = new Array(blocksCount);
                    for (var i = 0; i < blocksCount; i++) {
                        output[i] = this.children[i].text
                    }
                    return output
                }

                RegExpValidator {
                    id: regVal
                    regExp: /[0-9]{6}/
                }

                TextField { id: key01; objectName: "key01"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key02.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key02; objectName: "key02"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key03.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key03; objectName: "key03"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key04.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key04; objectName: "key04"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key05.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key05; objectName: "key05"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key06.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key06; objectName: "key06"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key07.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key07; objectName: "key07"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key08.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key08; objectName: "key08"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key09.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key09; objectName: "key09"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key10.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key10; objectName: "key10"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key11.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key11; objectName: "key11"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key12.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key12; objectName: "key12"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key13.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key13; objectName: "key13"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key14.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key14; objectName: "key14"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key15.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key15; objectName: "key15"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red"; if (text.length === 6) key16.forceActiveFocus(); } onActiveFocusChanged: if (activeFocus) selectAll(); }
                TextField { id: key16; objectName: "key16"; validator: regVal; width: 60; onTextChanged: { textColor = Utils.isPlausibleSingleBlock(text) ? "black" : "red";                                                  } onActiveFocusChanged: if (activeFocus) selectAll(); }
            }

            ErrorHint {
                id: loginFailedErrorHint
                anchors {
                    left: parent.left
                    leftMargin: 15
                }
                maxWidth: column1.width-anchors.leftMargin
            }

            LoginChecker {
                id: loginChecker
                client: Client.client()

                onLoginChecked: {
                    if (ok) {
                        console.info("Login ok.")
                        Client.userSettings.reset(address, masterKeyPem)
                        app.state = "loggingIn"
                    }
                    else {
                        console.info("Login failed.")
                        loginFailedErrorHint.text = qsTr("Login failed.")
                    }
                }
                onInvalidKulloAddress: {
                    loginFailedErrorHint.text = qsTr("Kullo address invalid.")
                }
                onInvalidMasterKey: {
                    loginFailedErrorHint.text = qsTr("MasterKey invalid.")
                }
                onServerError: {
                    loginFailedErrorHint.text = qsTr("Server error, please try again later.")
                }
                onProtocolError: {
                    loginFailedErrorHint.text = qsTr("Protocol error. This is our fault, sorry!")
                }
                onNetworkError: {
                    var host = Utils.splitKulloAddress(address)[1]
                    loginFailedErrorHint.text = qsTr("Server %1 could not be reached. Is the address correct? Are you online?")
                                                .arg(host)
                }
            }

            NativeButton {
                id: loginButton
                anchors {
                    right: parent.right
                }
                text: qsTr("Login")
                style: KulloButtonStyle {
                    source: "/resources/scalable/login_w.svg"
                }
                enabled: !loginChecker.locked
                onClicked: root.submit()
            }
        }
    }
}
