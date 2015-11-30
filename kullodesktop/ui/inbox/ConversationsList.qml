/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import Kullo 1.0

ListView {
    id: _conversationsList
    property int currentSecondaryIndex: -1
    property int lastSelectedConversationId: -1
    interactive: true

    focus: true
    clip: true

    function checkOpenWhenCreated()
    {
        var convId = Client.conversations.openWhenCreated();
        if (convId)
        {
            console.info("Conversation ID stored to be opened: " + convId);
            currentIndex = find(convId)
            inbox.openAnswer()
            return true
        }
        return false
    }

    onCurrentIndexChanged: {
        // Reset secondary highlight when main highlight changes
        currentSecondaryIndex = -1
    }

    function openConversation(convId)
    {
        var pos = find(convId)
        _conversationsList.currentIndex = pos
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
        if (_conversationsList.currentItem)
        {
            if (lastSelectedConversationId !== _conversationsList.currentItem.conversationId)
            {
                // lastSelectedConversationId must be set first because
                // inbox.openConversation() will trigger postConversationsChanged()
                // if messages are marked as read.
                lastSelectedConversationId = _conversationsList.currentItem.conversationId
                inbox.openConversation(_conversationsList.currentItem.conversationId)
            }
        }
    }

    function postConversationsChanged()
    {
        if (lastSelectedConversationId == -1) return
        if (checkOpenWhenCreated()) return
        var pos = find(lastSelectedConversationId);
        if (pos !== -1)
        {
            _conversationsList.currentIndex = pos
        }
        else
        {
            _conversationsList.lastSelectedConversationId = -1
            _conversationsList.currentIndex = -1
            openFirstConversationCheck()
        }
    }

    Connections {
        target: model
        onConversationsChanged: postConversationsChanged()
    }

    function find(convId)
    {
        if (count > 0)
        {
            for (var i = 0; i < count; ++i)
            {
                if (model.at(i)["id_"] === convId)
                {
                    return i;
                }
            }
        }
        console.warn("Conversation not found: id = " + convId)
        return -1;
    }

    property string quickKeySearchLastCharacter: ""
    property int quickKeySearchLastCharacterCount: 0

    function resetQuickKeySearch()
    {
        quickKeySearchLastCharacter = ""
        quickKeySearchLastCharacterCount = 0
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
                _conversationsList.currentIndex = 0
            }
        }
        else if (event.modifiers === 0 && event.key === Qt.Key_End) {
            event.accepted = true
            resetQuickKeySearch()
            if (Client.conversations.count() > 0)
            {
                _conversationsList.currentIndex = _conversationsList.count-1
            }
        }

        // a-zA-Z
        if (!event.accepted
                && (event.modifiers === 0 || event.modifiers === Qt.ShiftModifier)
                && (event.key >= Qt.Key_A && event.key <= Qt.Key_Z))
        {
            event.accepted = true
            var ch = event.text.toLowerCase()
            console.debug("Serach for contact: '" + ch + "'")
            var skip = 0
            if (quickKeySearchLastCharacter === ch)
            {
                skip = quickKeySearchLastCharacterCount
                quickKeySearchLastCharacterCount++
            }
            else {
                quickKeySearchLastCharacterCount = 1
            }
            quickKeySearchLastCharacter = ch

            var pos_first_found = -1
            for (var p = 0; p < count; ++p)
            {
                if (searchCharInConversation(ch, Client.conversations.at(p)))
                {
                    if (pos_first_found == -1) pos_first_found = p
                    if (skip == 0)
                    {
                        _conversationsList.currentIndex = p
                        return
                    }
                    else
                    {
                        skip -= 1
                    }
                }
            }

            if (pos_first_found != -1)
            {
                // not found anymore after `skip` results skipped
                quickKeySearchLastCharacterCount = 1
                _conversationsList.currentIndex = pos_first_found
            }
            else
            {
                // not found at all
                quickKeySearchLastCharacterCount = 0
            }
        }
    }

    Menu {
        id: _contextMenu
        property int selectedConversationId: -1
        property var participantsAddresses: (selectedConversationId != -1)
                ? _conversationsList.model.get(selectedConversationId).participantsAddresses
                : null

        MenuItem {
            text: qsTr("Info")
            onTriggered: {
                conversationInfoWindow.conversationId = _contextMenu.selectedConversationId
                conversationInfoWindow.participantsAddresses = _conversationsList.model.get(_contextMenu.selectedConversationId).participantsAddresses
                conversationInfoWindow.participants = _conversationsList.model.get(_contextMenu.selectedConversationId).participants
                conversationInfoWindow.openWindow()
            }
        }

        MenuItem {
            text: qsTr("Copy Kullo address")

            visible: _contextMenu.participantsAddresses
                     && _contextMenu.participantsAddresses.length === 1

            onTriggered: {
                var address = _contextMenu.participantsAddresses[0];
                Utils.setClipboardText(address)
            }
        }

        Menu {
            id: addressEntries
            title: qsTr("Copy Kullo address")
            visible: _contextMenu.participantsAddresses
                     && _contextMenu.participantsAddresses.length > 1

            Instantiator {
                model: _contextMenu.participantsAddresses
                MenuItem {
                    property string address: model.modelData
                    text: address
                    onTriggered: Utils.setClipboardText(address)
                }
                onObjectAdded: addressEntries.insertItem(index, object)
                onObjectRemoved: addressEntries.removeItem(object)
            }
        }

        MenuItem {
            text: qsTr("Remove")
            enabled: _contextMenu.selectedConversationId != -1
                     && _conversationsList.model.get(_contextMenu.selectedConversationId).count === 0
            onTriggered: {
                var convId = _contextMenu.selectedConversationId
                if (Client.removeConversation(convId)) {
                    console.debug("Conversation removed: " + convId)
                }
                else {
                    console.error("Conversation couldn't be removed: " + convId)
                }
                inbox.closeAnswer()
            }
        }
    }

    onActiveFocusChanged: {
        focusChanged(focus)
    }

    delegate: ConversationDelegate {
        id: listItem
        hasSecondaryHighlight: _conversationsList.currentSecondaryIndex === index

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: {
                if (mouse.button == Qt.LeftButton)
                {
                    inbox.closeAnswer()
                    _conversationsList.currentIndex = index
                    _conversationsList.forceActiveFocus()
                }
                else if (mouse.button == Qt.RightButton)
                {
                    _conversationsList.currentSecondaryIndex = index
                    _contextMenu.selectedConversationId = id_
                    _contextMenu.popup()
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
