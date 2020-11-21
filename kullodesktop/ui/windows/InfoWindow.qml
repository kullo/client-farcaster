/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import Kullo 1.0

import ".."
import "../buttons"
import "../misc"
import "../native"
import "../js/format.js" as Format
import "../js/shortcut.js" as SC

NativeModalWindow {
    id: root
    title: qsTr("Info")
    objectName: "InfoWindow"

    signal downloadUpdate()

    /* private */
    property int _DEFAULT_WINDOW_WIDTH: 310 * Hdpi.FontScalingFactor
    property int _DEFAULT_WINDOW_HEIGHT: 460 * Hdpi.FontScalingFactor

    property int _PADDING: 12 * Hdpi.FontScalingFactor

    width: _DEFAULT_WINDOW_WIDTH
    height: _DEFAULT_WINDOW_HEIGHT

    onVisibleChanged: {
        if (visible) {
            // reset window dimensions
            root.width = _DEFAULT_WINDOW_WIDTH
            root.height = _DEFAULT_WINDOW_HEIGHT

            mainItem.forceActiveFocus()

            KulloVersionChecker.run() // check again in case user was offline during last check
        }
    }

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors {
            fill: parent
            margins: _PADDING
        }

        Keys.onEscapePressed: root.closeWindow()
        Keys.onPressed: handleNativeWindowShortcuts(event)

        Column {
            anchors {
                fill: parent
                topMargin: 10 * Hdpi.FontScalingFactor
            }
            spacing: 25 * Hdpi.FontScalingFactor

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
                id: updateAvailableBlock
                visible: KulloVersionChecker.updateAvailable
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: 7 * Hdpi.FontScalingFactor

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
                    text: qsTr("Or visit our <a href='%1'>download website</a>.")
                          .arg(KulloVersionChecker.updateUrl)
                }
            }

            Column {
                visible: !updateAvailableBlock.visible
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: 7 * Hdpi.FontScalingFactor

                NativeText {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    property var date: KulloVersionChecker.lastSuccessfulCheck

                    function dateIsValid(date) {
                        return date && !isNaN(date.getTime())
                    }

                    property string dateStr: dateIsValid(date)
                                             ? Format.humanDatetime(date, Locale.ShortFormat, Locale.ShortFormat)
                                             : "–"

                    property bool recentlyChecked: dateIsValid(date)
                                                   && ((new Date()).getTime() - date.getTime()) < 24*3600*1000

                    text: (recentlyChecked ? qsTr("Kullo is up to date, yeah!") + " " : "")
                          + qsTr("Last version check: %1").arg(dateStr)
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
                text: qsTr("Kullo is proudly based on open source software.") + " "
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
                text: qsTr("Kullo for %1 is provided by Kullo&nbsp;GmbH").arg(Os.familyPretty)
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
