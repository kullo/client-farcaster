/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3

import "../"
import "../buttons"
import "../native"
import "../js/shortcut.js" as SC

FocusScope {
    /* public */
    property alias conversation: answer.conversation
    property alias buttonBoxState: buttonBox.state
    property alias animationEnabled: fadeInOutAnimation.enabled

    function addQuoteToAnswer(quoteText) {
        answer.addQuoteToAnswer(quoteText)
    }

    /* private */
    clip: false

    Behavior on width {
        id: fadeInOutAnimation
        enabled: true
        NumberAnimation { duration: 150 }
    }

    Rectangle {
        id: buttonBox
        anchors {
            right: parent.left
            // one extra pixel to the right to cover rounding errors
            rightMargin: -1
            top: parent.top
            topMargin: 5 * Hdpi.FontScalingFactor
        }

        height: closeButton.height
        color: "transparent"
        clip: true

        Behavior on width { NumberAnimation { duration: 50 } }
        states: [
            State {
                name: "default"
                PropertyChanges {
                    target: buttonBox
                    width: 0
                }
            },
            State {
                name: "answerOpen"
                PropertyChanges {
                    target: buttonBox
                    // one extra pixel to the right to cover rounding errors
                    width: 36 * Hdpi.FontScalingFactor + 1
                }
            }
        ]
        state: "default"

        NativeButton {
            id: closeButton
            anchors {
                left: parent.left
                top: parent.top
            }
            tooltip: qsTr("Close reply")
                     + " (%1)".arg(SC.nameOfCtrlAndKey("R", Os.osx))
            style: KulloButtonStyle {
                backgroundColor: Style.answerColumnBackground
                hoverColor: "#22000000"
                borderRadiusTopRight: 0
                borderRadiusBottomRight: 0
                iconPosition: _POSITION_LEFT
                source: "/resources/scalable/close_b.svg"
                paddingH: 8 * Hdpi.FontScalingFactor
            }
            onClicked: inboxScreen.closeAnswer()
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        clip: true
        color: Style.answerColumnBackground

        AnswerBox {
            id: answer

            anchors {
                fill: parent
                topMargin: 10 * Hdpi.FontScalingFactor
                rightMargin: 15 * Hdpi.FontScalingFactor
                bottomMargin: 10 * Hdpi.FontScalingFactor
                leftMargin: 15 * Hdpi.FontScalingFactor
            }

            focus: true
        }
    }
}
