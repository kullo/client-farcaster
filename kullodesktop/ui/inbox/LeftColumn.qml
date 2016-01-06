/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

import "../"
import "../windows"

FocusScope {
    id: scope

    function openConversation(convId)
    {
        conversationsList.openConversation(convId)
    }

    Behavior on width {
        NumberAnimation {
            duration: 100
        }
    }

    ConversationInfoWindow {
        id: conversationInfoWindow
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
