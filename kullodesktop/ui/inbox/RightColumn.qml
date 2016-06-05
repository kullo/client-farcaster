/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
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
            topMargin: 5
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
                    width: 36 + 1
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
                paddingH: 8
            }
            onClicked: inbox.closeAnswer()
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
                topMargin: 10
                rightMargin: 15
                bottomMargin: 10
                leftMargin: 15
            }

            focus: true
        }
    }
}
