/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4

import ".."
import "../buttons"
import "../native"
import "../js/shortcut.js" as SC

BackgroundCover {
    state: "hidden"

    Rectangle {
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        width: 520
        height: 320
        radius: 10

        MouseArea {
            anchors.fill: parent
            onClicked: {} // do nothing. Prevents overlay from closing
        }

        IconButton {
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 7
                rightMargin: 10
            }
            tooltip: qsTr("Close (Esc)")
            source: "/resources/scalable/close2_b.svg"
            onClicked: fadeOut()
        }

        Column {
            anchors {
                fill: parent
                topMargin: 25
                leftMargin: 15
                rightMargin: 15
                bottomMargin: 15
            }
            spacing: 20

            NativeText {
                text: qsTr("Use Kullo like a pro")
                anchors {
                    left: parent.left
                    right: parent.right
                }
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: Style.fontSize.shortcutOverlayHeadline
            }

            Column {
                spacing: 7
                anchors {
                    left: parent.left
                    right: parent.right
                }

                ShortcutRow {
                    ShortcutBlock {
                        CtrlKey {}
                        NativeText { text: "+" }
                        Key { text: "N" }
                    }

                    NativeText {
                        text: qsTr("Create new conversation")
                    }
                }

                ShortcutRow {
                    ShortcutBlock {
                        CtrlKey {}
                        NativeText { text: "+" }
                        Key { text: "T" }
                    }

                    NativeText {
                        text: qsTr("Switch Todo mode on/off")
                    }
                }

                ShortcutRow {
                    ShortcutBlock {
                        id: sc
                        Key { text: "a" }
                        NativeText { text: "–" }
                        Key { text: "z" }
                    }

                    NativeText {
                        width: parent.width - parent.spacing - sc.width
                        text: qsTr("Jump to next conversation starting with the letter")
                    }
                }

                ShortcutRow {
                    ShortcutBlock {
                        CtrlKey {}
                        NativeText { text: "+" }
                        Key { text: "O" }
                    }

                    NativeText {
                        text: qsTr("Leave current inbox")
                    }
                }

                ShortcutRow {
                    ShortcutBlock {
                        CtrlKey {}
                        NativeText { text: "+" }
                        Key { text: "R" }
                    }

                    NativeText {
                        text: qsTr("Reply to the current conversation")
                    }
                }

                ShortcutRow {
                    ShortcutBlock {
                        CtrlKey {}
                        NativeText { text: "+" }
                        ReturnKey { }
                    }

                    NativeText {
                        text: qsTr("Send message")
                    }
                }

                ShortcutRow {
                    ShortcutBlock {
                        CtrlKey {}
                        NativeText { text: "+" }
                        Key { text: "Q" }
                    }

                    NativeText {
                        text: qsTr("Exit Kullo")
                    }
                }

                ShortcutRow {
                    ShortcutBlock {
                        Key { text: "?" }
                    }

                    NativeText {
                        text: qsTr("Show this shortcut page")
                    }
                }
            }
        }
    }
}
