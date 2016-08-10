/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

ListView {
    /* private */
    property int _currentSecondaryIndex: -1
    property int _lastSelectedConversationId: -1

    id: root
    interactive: true

    focus: true
    clip: true

    function checkOpenWhenCreated()
    {
        var convId = Client.conversations.openWhenCreated();
        if (convId < 0) return false;

        console.info("Conversation ID stored to be opened: " + convId);
        currentIndex = model.find(convId)
        inbox.openAnswer()
        return true
    }

    onCurrentIndexChanged: {
        // Reset secondary highlight when main highlight changes
        _currentSecondaryIndex = -1
    }

    function openConversation(convId)
    {
        var pos = model.find(convId)
        root.currentIndex = pos
    }

    function openFirstConversationCheck()
    {
        if (currentIndex == -1 && count > 0)
        {
            console.info("No conversation selected, " + count + " available. "
                         + "Open 1st conversation ...")
            currentIndex = 0
            console.info("Open 1st conversation. Done. Id: " + currentItem.conversationId)
            forceActiveFocus()
        }
    }

    onCountChanged: openFirstConversationCheck()

    onCurrentItemChanged: {
        if (root.currentItem)
        {
            if (_lastSelectedConversationId !== root.currentItem.conversationId)
            {
                // lastSelectedConversationId must be set first because
                // inbox.openConversation() will trigger postConversationsChanged()
                // if messages are marked as read.
                _lastSelectedConversationId = root.currentItem.conversationId
                inbox.openConversation(root.currentItem.conversationId)
            }
        }
    }

    function postConversationsChanged()
    {
        if (_lastSelectedConversationId == -1) return
        if (checkOpenWhenCreated()) return
        var pos = model.find(_lastSelectedConversationId);
        if (pos !== -1)
        {
            root.currentIndex = pos
        }
        else
        {
            _lastSelectedConversationId = -1
            root.currentIndex = -1
            openFirstConversationCheck()
        }
    }

    Connections {
        target: model
        onConversationsChanged: postConversationsChanged()
    }

    property string quickKeySearchLastCharacter: ""
    property int quickKeySearchLastCharacterPosition: 0

    function resetQuickKeySearch()
    {
        quickKeySearchLastCharacter = ""
        quickKeySearchLastCharacterPosition = 0
    }

    function searchCharInConversation(character, conv)
    {
        var participants = conv['participants_']

        for (var addr in participants)
        {
            if (addr.substring(0, 1) === character)
            {
                return true
            }

            var name = participants[addr].trim().toLowerCase();
            if (name !== "")
            {
                var name_parts = name.split(' ', 5)

                for (var index in name_parts)
                {
                    var part = name_parts[index].trim()

                    if (part.substring(0, 1) === character)
                    {
                        return true
                    }
                }
            }
        }

        return false
    }

    Keys.onPressed: {
        if (event.modifiers === 0 && event.key === Qt.Key_Home) {
            event.accepted = true
            resetQuickKeySearch()
            if (Client.conversations.count() > 0)
            {
                root.currentIndex = 0
            }
        }
        else if (event.modifiers === 0 && event.key === Qt.Key_End) {
            event.accepted = true
            resetQuickKeySearch()
            if (Client.conversations.count() > 0)
            {
                root.currentIndex = root.count-1
            }
        }

        // a-zA-Z
        if (!event.accepted
                && (event.modifiers === 0 || event.modifiers === Qt.ShiftModifier)
                && (event.key >= Qt.Key_A && event.key <= Qt.Key_Z))
        {
            event.accepted = true
            var ch = event.text.toLowerCase()
            var reverseDirection = (event.modifiers === Qt.ShiftModifier)
            console.debug("Search for contact: '" + ch + "' "
                          + (reverseDirection ? 'up' : 'down'))

            var hitsCount = 0
            var skipsRemaining = 0
            if (quickKeySearchLastCharacter === ch)
            {
                if (reverseDirection)
                    quickKeySearchLastCharacterPosition--
                else
                    quickKeySearchLastCharacterPosition++

                skipsRemaining = quickKeySearchLastCharacterPosition - 1
            }
            else
            {
                // User changed quick search character
                quickKeySearchLastCharacter = ch
                quickKeySearchLastCharacterPosition = 1
            }

            var posFirstFind = -1
            var posLastFind = -1
            for (var p = 0; p < count; ++p)
            {
                if (searchCharInConversation(ch, Client.conversations.at(p)))
                {
                    if (posFirstFind == -1) posFirstFind = p
                    if (p > posLastFind) posLastFind = p

                    if (skipsRemaining == 0)
                    {
                        root.currentIndex = p
                        return
                    }
                    else if (skipsRemaining > 0)
                    {
                        skipsRemaining -= 1
                    }

                    hitsCount++
                }
            }

            if (posFirstFind === -1)
            {
                // character not found at all
                resetQuickKeySearch()
            }
            else if (reverseDirection)
            {
                root.currentIndex = posLastFind
                quickKeySearchLastCharacterPosition = hitsCount
            }
            else
            {
                // not found anymore after skipping too many results:
                // Loop back to the top
                quickKeySearchLastCharacterPosition = 1
                root.currentIndex = posFirstFind
            }
        }
    }

    Menu {
        id: contextMenu
        property int selectedConversationId: -1
        property var participantsAddresses: (selectedConversationId != -1)
                ? root.model.get(selectedConversationId).participantsAddresses
                : null

        MenuItem {
            text: qsTr("Info")
            onTriggered: {
                conversationInfoWindow.conversationId = contextMenu.selectedConversationId
                conversationInfoWindow.participantsAddresses = root.model.get(contextMenu.selectedConversationId).participantsAddresses
                conversationInfoWindow.participants = root.model.get(contextMenu.selectedConversationId).participants
                conversationInfoWindow.openWindow()
            }
        }

        MenuItem {
            text: qsTr("Copy Kullo address")

            visible: contextMenu.participantsAddresses
                     && contextMenu.participantsAddresses.length === 1

            onTriggered: {
                var address = contextMenu.participantsAddresses[0];
                Utils.setClipboardText(address)
            }
        }

        Menu {
            id: addressEntries
            title: qsTr("Copy Kullo address")
            visible: contextMenu.participantsAddresses
                     && contextMenu.participantsAddresses.length > 1

            Instantiator {
                model: contextMenu.participantsAddresses
                MenuItem {
                    /* private */
                    property string _address: model.modelData

                    text: _address
                    onTriggered: Utils.setClipboardText(_address)
                }
                onObjectAdded: addressEntries.insertItem(index, object)
                onObjectRemoved: addressEntries.removeItem(object)
            }
        }

        MenuItem {
            text: qsTr("Remove")
            enabled: contextMenu.selectedConversationId != -1
                     && root.model.get(contextMenu.selectedConversationId).count === 0
            onTriggered: {
                var convId = contextMenu.selectedConversationId
                Client.removeConversation(convId)
                inbox.closeAnswer()
            }
        }
    }

    onActiveFocusChanged: {
        focusChanged(focus)
    }

    delegate: ConversationDelegate {
        id: listItem
        hasSecondaryHighlight: root._currentSecondaryIndex === index

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: {
                if (mouse.button === Qt.LeftButton)
                {
                    inbox.closeAnswer()
                    root.currentIndex = index
                    root.forceActiveFocus()
                }
                else if (mouse.button === Qt.RightButton)
                {
                    root._currentSecondaryIndex = index
                    contextMenu.selectedConversationId = id_
                    contextMenu.popup()
                }
            }
        }
    }

    model: Client.conversations

    Component.onCompleted: {
        Client.conversations.todoMode = Qt.binding(function() { return inbox.todoMode })
    }

    highlight: ConversationHighlighter {}
    highlightMoveDuration: 50
}
