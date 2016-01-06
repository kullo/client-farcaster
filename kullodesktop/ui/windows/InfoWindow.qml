/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import Kullo 1.0

import ".."
import "../buttons"
import "../native"
import "../js/shortcut.js" as SC

NativeModalWindow {
    id: _root
    width: 300
    height: 450
    title: qsTr("Info")
    objectName: "InfoWindow"

    signal downloadUpdate()

    onVisibleChanged: if (visible) mainItem.forceActiveFocus()

    // Main Qt Quick Item required for attached property `Keys`
    Item {
        id: mainItem
        anchors {
            fill: parent
            margins: 10
        }

        Keys.onEscapePressed: _root.closeWindow()
        Keys.onPressed: handleCtrlW(event)

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
                    text: SoftwareVersions.version
                    horizontalAlignment: Text.AlignHCenter
                    // if this text wraps, layout is destroyed entirely
                    wrapMode: Text.NoWrap
                    font.pointSize: Style.fontSize.kulloVersion
                    font.bold: true
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
                     onClicked: _root.downloadUpdate()
                }

                NativeText {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    textFormat: Text.RichText
                    text: qsTr("Or visit our <a href='%1' style='color: %2'>download website</a>.")
                          .arg(KulloVersionChecker.updateUrl)
                          .arg(color)
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }

            NativeText {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                textFormat: Text.RichText
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
                      + qsTr("Read all <a href='%1' style='color: %2'>licenses</a>.")
                        .arg(Utils.makeFileUrl("assets/licenses.html"))
                        .arg(color)
                onLinkActivated: Qt.openUrlExternally(link)
            }

            NativeText {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                textFormat: Text.RichText
                text: qsTr("Kullo for %1 is provided by Kullo&nbsp;GmbH").arg(Os.familyPretty)
                      + ": "
                      + "<a href='https://www.kullo.net/' style='color: %1'>www.kullo.net</a>"
                      .arg(color)
                onLinkActivated: Qt.openUrlExternally(link)
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
            onClicked: _root.closeWindow()
            style: KulloButtonStyle {}
        }
    }
}
