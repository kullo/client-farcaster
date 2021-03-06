/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import Kullo 1.0

import "../"
import "../buttons"
import "../native"
import "../misc"
import "../js/shortcut.js" as SC

Rectangle {
    height: profileButton.implicitHeight + 2*5 * Hdpi.FontScalingFactor
    color: Style.leftToolbarBackground

    // delay loading data to speed up software start
    DelayedRepeatingTimer {
        firstInterval: 1000 // 1 second
        secondInterval: 5*60*1000 // 5 minutes
        handleOnTriggered: function() {
            superHandleOnTriggered()
            accountInfo.reload()
        }
    }

    AccountInfo {
        id: accountInfo
        inbox: Inbox
    }

    NativeButton {
        id: profileButton
        anchors {
            top: parent.top
            topMargin: 5 * Hdpi.FontScalingFactor
            left: parent.left
            leftMargin: 5 * Hdpi.FontScalingFactor
            right: moreButton.left
            rightMargin: 5 * Hdpi.FontScalingFactor
        }

        text: qsTr("My Kullo")

        style: KulloButtonStyle {
            function reloadAvatar() {
                source = "image://usersettingsavatars/" + Inbox.userSettings.address + "?circle&" + Date.now()
            }

            source: "image://usersettingsavatars/" + Inbox.userSettings.address + "?circle"
            iconSize: 32 * Hdpi.FontScalingFactor
            iconPosition: _POSITION_LEFT
            paddingH: 8 * Hdpi.FontScalingFactor
            paddingV: 2 * Hdpi.FontScalingFactor
            backgroundColor: "transparent"
            hoverColor: "#22000000"
            textColor: Style.black
            progress: accountInfo.storageUsed
            progressMax: accountInfo.storageQuota

            Connections {
                target: profileOverlay
                onAvatarChanged: reloadAvatar()
            }
        }

        onClicked: {
            popupMenu.close()
            profileOverlay.fadeIn()
        }
    }

    IconButton {
        tooltip: qsTr("More menu items")
        id: moreButton
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 5 * Hdpi.FontScalingFactor
        }
        source: "/resources/scalable/more_b.svg"
        badgeEnabled: KulloVersionChecker.updateAvailable

        onClicked: {
            if (!popupMenu.opened)
            {
                var y = 0
                var x = width/2
                var targetPoint = mapToItem(inboxContent, x, y)
                popupMenu.placeAt(targetPoint)
                popupMenu.open()
            }
            else
            {
                popupMenu.close()
            }
        }
    }
}
