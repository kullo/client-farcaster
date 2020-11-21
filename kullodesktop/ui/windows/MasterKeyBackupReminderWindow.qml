/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3

import ".."
import "../buttons"
import "../formelements"
import "../native"

NativeModalWindow {
    id: root
    width: 500 * Hdpi.FontScalingFactor
    height: 410 * Hdpi.FontScalingFactor
    title: qsTr("MasterKey backup notification")
    objectName: "MasterKeyBackupReminderWindow"

    // Set dirty when window is opened because of
    // https://bugreports.qt.io/browse/QTBUG-44720
    property bool _dirtyClosing: false

    onVisibleChanged: {
        if (visible) // window opened
        {
            root._dirtyClosing = true
            mainItem.forceActiveFocus()
        }
    }

    function postpone(seconds)
    {
        Inbox.userSettings.postponeMasterKeyBackupDontRemindBefore(seconds)
        _dirtyClosing = false
    }

    function confirm()
    {
        Inbox.userSettings.confirmMasterKeyBackup()
        _dirtyClosing = false
    }

    onClosing: {
        if (_dirtyClosing)
        {
            console.info("MasterKeyBackupReminderWindow is dirty closing. Postpone for 1 hour")
            postpone(1 * 3600) // Come back in 1 hours
        }
    }

    MasterKeyShowWindow {
        id: masterKeyShowWindow
        address: Inbox.userSettings.address
        key: Inbox.userSettings.masterKeyPem
    }

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors {
            fill: parent
        }

        Keys.onEscapePressed: root.closeWindow()
        Keys.onPressed: handleNativeWindowShortcuts(event)

        Rectangle {
            id: header
            property int paddingH: 10 * Hdpi.FontScalingFactor
            property int paddingV: 10 * Hdpi.FontScalingFactor

            color: Qt.darker(root.color, 1.20)
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: laterButton.height + 2*paddingV

            Flow {
                anchors {
                    left: parent.left
                    leftMargin: parent.paddingH
                    right: laterButton.left
                    rightMargin: parent.paddingH
                    verticalCenter: parent.verticalCenter
                }

                ExclusiveGroup {
                    id: remindTime
                }

                spacing: 5 * Hdpi.FontScalingFactor

                NativeText {
                    id: remind
                    height: firstRadioButton.implicitHeight
                    text: qsTr("Remind in")
                    verticalAlignment: Text.AlignVCenter
                }

                RadioButton {
                    id: firstRadioButton
                    exclusiveGroup: remindTime
                    property int seconds: 3 * 3600
                    text: qsTr("%1 hours").arg(3)
                }

                RadioButton {
                    exclusiveGroup: remindTime
                    property int seconds: 24 * 3600
                    text: qsTr("1 day")
                    checked: true
                }

                RadioButton {
                    exclusiveGroup: remindTime
                    property int seconds: 7 * 24 * 3600
                    text: qsTr("1 week")
                }
            }

            NativeButton {
                id: laterButton
                anchors {
                    right: parent.right
                    rightMargin: parent.paddingH
                    bottom: parent.bottom
                    bottomMargin: parent.paddingV
                }
                text: qsTr("Later")
                onClicked: {
                    var seconds = remindTime.current.seconds
                    postpone(seconds)
                    root.closeWindow()
                }
                style: KulloButtonStyle {
                    source: "/resources/scalable/later_w.svg"
                }
            }
        }

        FForm {
            anchors {
                top: header.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                topMargin: 30 * Hdpi.FontScalingFactor
                leftMargin: 10 * Hdpi.FontScalingFactor
                rightMargin: 10 * Hdpi.FontScalingFactor
                bottomMargin: 10 * Hdpi.FontScalingFactor
            }

            Column {
                anchors {
                    fill: parent
                }
                spacing: 20 * Hdpi.FontScalingFactor

                NativeText {
                    text: qsTr("If your MasterKey is lost,\nyour messages are lost.")
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    // if this text wraps, layout is destroyed entirely
                    wrapMode: Text.NoWrap
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: Style.fontSize.masterKeyBackupHeadline
                    font.bold: true
                }

                NativeText {
                    text: qsTr("Kullo's end-to-end encryption allows decrypting messages only for those who are in possession of your MasterKey. There is no recovery that would allow a Kullo provider to reset a lost MasterKey. To ensure that accessing your Kullo inbox is still possible if your device crashes or is stolen, please backup your MasterKey on paper.")
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                }

                NativeText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    textFormat: Text.StyledText
                    text: qsTr("<a href='showMasterKey://'>Show MasterKey and print now</a>")
                    onLinkActivated: {
                        confirmation.enabled = true

                        masterKeyShowWindow.openWindow()
                        masterKeyShowWindow.raise()
                        masterKeyShowWindow.requestActivate()
                    }
                }
            }

            Column {
                id: confirmation
                enabled: false
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                spacing: 10 * Hdpi.FontScalingFactor

                CheckBox {
                    id: checkBox
                    anchors {
                        left: parent.left
                    }
                    enabled: confirmation.enabled
                    text: qsTr("I swear, I did backup my MasterKey!")
                }

                NativeButton {
                    id: doneButton
                    anchors {
                        right: parent.right
                    }
                    enabled: confirmation.enabled && checkBox.checked
                    text: qsTr("Done")
                    style: KulloButtonStyle {
                        source: "/resources/scalable/ok_w.svg"
                    }
                    onClicked: {
                        confirm()
                        root.closeWindow()
                    }
                }
            }
        }
    }
}
