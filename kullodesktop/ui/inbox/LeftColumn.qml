/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

import "../"
import "../windows"

FocusScope {
    function selectConversation(convId) {
        conversationsList.selectConversation(convId)
    }

    Behavior on width {
        NumberAnimation {
            duration: 100
        }
    }

    Rectangle {
        id: columnBackground
        anchors.fill: parent
        color: Style.conversationsListBackground

        onActiveFocusChanged: {
            focusChanged(focus)
        }

        ToolbarTop {
            id: header
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
        }

        ConversationsList {
            id: conversationsList
            anchors {
                top: header.bottom
                bottom: footer.top
            }
            width: parent.width
        }

        ToolbarBottom {
            id: footer
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
        }
    }
}
