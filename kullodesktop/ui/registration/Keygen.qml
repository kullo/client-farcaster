/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.2

import "../buttons"
import "../native"

Item {
    id: root
    property var registerer: null;

    signal done()

    anchors {
        fill: parent
    }

    Connections {
        target: registerer

        onKeysGenerationProgressChanged: {
            progressBar.value = percent
            console.debug("------->" + percent + "%");

            if (percent == 100)
            {
                root.done()
            }
        }
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
        }

        spacing: 30 * Hdpi.FontScalingFactor

        BackButton {
            onClicked: app.state = "welcome"
        }

        NativeText {
            anchors {
                left: parent.left
                right: parent.right
            }
            font.pointSize: 16
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Kullo is searching primes for you. Big primes.")
        }

        ProgressBar {
            id: progressBar
            anchors {
                left: parent.left
                right: parent.right
            }
            minimumValue: 0
            maximumValue: 100
            value: 0
        }

        NativeText {
            anchors {
                left: parent.left
                right: parent.right
            }
            text: qsTr("This might take up to 3 minutes but is important for your security.")
        }
    }
}
