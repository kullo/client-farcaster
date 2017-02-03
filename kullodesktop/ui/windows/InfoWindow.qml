/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import Kullo 1.0

import ".."
import "../buttons"
import "../misc"
import "../native"
import "../js/shortcut.js" as SC

NativeModalWindow {
    id: root
    title: qsTr("Info")
    objectName: "InfoWindow"

    signal downloadUpdate()

    /* private */
    property int _DEFAULT_WINDOW_WIDTH: 300
    property int _DEFAULT_WINDOW_HEIGHT: 450

    width: _DEFAULT_WINDOW_WIDTH
    height: _DEFAULT_WINDOW_HEIGHT

    onVisibleChanged: {
        if (visible) {
            // reset window dimensions
            root.width = _DEFAULT_WINDOW_WIDTH
            root.height = _DEFAULT_WINDOW_HEIGHT

            mainItem.forceActiveFocus()
        }
    }

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors {
            fill: parent
            margins: 10
        }

        Keys.onEscapePressed: root.closeWindow()
        Keys.onPressed: handleNativeWindowShortcuts(event)

        Column {
            anchors {
                fill: parent
                margins: 10
            }
            spacing: 30

            Column {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                NativeText {
                    id: kulloVersionLabel
                    text: qsTr("Version")
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    horizontalAlignment: Text.AlignHCenter
                }

                NativeText {
                    id: kulloVersion
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: SoftwareVersions.versionSimplified
                    horizontalAlignment: Text.AlignHCenter
                    // if this text wraps, layout is destroyed entirely
                    wrapMode: Text.NoWrap
                    font.pointSize: Style.fontSize.kulloVersion
                    font.bold: true

                    TooltipArea {
                        text: SoftwareVersions.version
                    }
                }
            }

            Column {
                visible: KulloVersionChecker.updateAvailable
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: 7

                NativeText {
                    id: downloadText
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: qsTr("Kullo version %1 is available.").arg(KulloVersionChecker.versionAvailable)
                }

                NativeButton {
                     id: downloadButton
                     anchors {
                         horizontalCenter: parent.horizontalCenter
                     }
                     text: qsTr("Download")
                     onClicked: root.downloadUpdate()
                }

                NativeTextWithLinks {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    html: qsTr("Or visit our <a href='%1'>download website</a>.")
                          .arg(KulloVersionChecker.updateUrl)
                }
            }

            NativeTextWithLinks {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                function composeVersions()
                {
                    var out = ""
                    var list = SoftwareVersions.list
                    out = list[0]
                    for (var pos = 1; pos <= list.length - 2; ++pos)
                    {
                        out += ", " + list[pos]
                    }
                    out += qsTr(" and ") + list[list.length-1]
                    return out
                }
                html: qsTr("Kullo is proudly based on open source software.") + " "
                      + qsTr("Kudos to %1.").arg(composeVersions())
                      + " "
                      + qsTr("Read all <a href='%1'>licenses</a>.")
                        .arg(Utils.makeFileUrl("assets/licenses.html"))
            }

            NativeTextWithLinks {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                html: qsTr("Kullo for %1 is provided by Kullo&nbsp;GmbH").arg(Os.familyPretty)
                      + ": "
                      + "<a href='https://www.kullo.net/'>www.kullo.net</a>"
            }
        }

        NativeButton {
            anchors {
                right: parent.right
                bottom: parent.bottom
            }
            text: qsTr("Okay")
            tooltip: qsTr("Close window")
                     + " (%1)".arg(SC.nameOfCtrlAndKey("W", Os.osx))
            onClicked: root.closeWindow()
            style: KulloButtonStyle {}
        }
    }
}
