/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
import QtQuick 2.5
import QtQuick.Controls 1.4

import ".."
import "../dialogs"
import "../native"

ScrollView {
    id: root

    function openInbox(address) {
        Inbox.loadCredentials(address)
        app.state = "loggingIn"
    }

    ListView {
        id: accountsListView
        clip: true
        interactive: false
        anchors.fill: parent
        focus: true // in focus scope ScrollView

        highlight: Rectangle {
            color: Style.welcomeAccountsSecondaryHighlightedBackground
        }
        highlightMoveDuration: 0

        section.property: "sectionId_"
        section.delegate: Item {
            height: 50 * Hdpi.FontScalingFactor
            width: accountsListView.width

            NativeText {
                anchors {
                    left: parent.left
                    leftMargin: 10 * Hdpi.FontScalingFactor
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                text: qsTr("Logged in accounts")
                font.bold: true
                font.pointSize: Style.fontSize.big
            }
        }

        function createNewListModel() {
            return Qt.createQmlObject("import QtQuick 2.5; ListModel {}", accountsListView);
        }

        function reloadModel() {
            var listModel = createNewListModel()
            var addresses = InnerApplication.deviceSettings.usersList()
            for (var i = 0; i < addresses.length; ++i)
            {
                var address = addresses[i]
                listModel.append({ "sectionId_": 0, "address_": address })
            }
            model = listModel

            currentIndex = (addresses.length > 0) ? 0 : -1
        }
        Component.onCompleted: reloadModel()

        property bool contextMenuVisible: false

        Keys.onPressed: {
            if (event.accepted) return

            // Key_Enter always comes with KeypadModifier
            if (event.modifiers === Qt.NoModifier || event.modifiers === Qt.KeypadModifier) {
                if (event.key === Qt.Key_Up) {
                    if (currentIndex != -1) {
                        currentIndex = Math.max(currentIndex-1, 0)
                    }
                    event.accepted = true
                }
                else if (event.key === Qt.Key_Down) {
                    if (currentIndex != -1) {
                        currentIndex = Math.min(currentIndex+1, count-1)
                    }
                    event.accepted = true
                } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    var address = currentItem.address // currentItem is of type AccountDelegate
                    root.openInbox(address)
                    event.accepted = true
                }
            }
        }

        delegate: AccountDelegate {
            id: delegate
            height: 40 * Hdpi.FontScalingFactor
            width: accountsListView.width
            address: address_

            YesNoDialog {
                id: logoutConfirmationDialog
                objectName: "logoutConfirmation"
                width: 400 * Hdpi.FontScalingFactor
                title: qsTr("Log out and delete data")
                text: qsTr("Make sure you have a backup of your MasterKey before logging out. " +
                           "Otherwise, you will lose access to your account!") +
                      "\n\n" +
                      qsTr("Do you really want to log out?")
                onYes: {
                    console.info("Deleting account data of '" + delegate.address + "' ...")
                    Inbox.deleteAccountData(delegate.address)
                    accountsListView.reloadModel()
                }
            }

            onLogoutRequested: logoutConfirmationDialog.openDialog()
            onOpenRequested: root.openInbox(address)
            onSelectRequested: {
                accountsListView.forceActiveFocus()
                accountsListView.currentIndex = index
            }
            onAboutToShowContextMenu: accountsListView.contextMenuVisible = true
            onAboutToHideContextMenu: accountsListView.contextMenuVisible = false
        }
    }
}
