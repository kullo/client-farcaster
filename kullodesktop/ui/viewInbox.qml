/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtMultimedia 5.0
import Kullo 1.0

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "inbox"
import "inbox/banners"
import "dialogs"
import "misc"
import "native"
import "usersettings"
import "windows"
import "js/dynamic.js" as Dynamic
import "js/shortcut.js" as SC

FocusScope {
    id: inbox

    anchors.fill: parent
    property bool todoMode: false
    property bool storeWindowSizeChanges: false

    onWidthChanged: reactToWindowDimensionsChanged()
    onHeightChanged: reactToWindowDimensionsChanged()

    Connections {
        target: appWindow
        onVisibilityChanged: reactToWindowDimensionsChanged()
    }

    function reactToWindowDimensionsChanged() {
        // http://doc.qt.io/qt-5/qwindow.html#Visibility-enum
        var QWindow_Hidden = 0
        var QWindow_Windowed = 2
        var QWindow_Minimized = 3
        var QWindow_Maximized = 4

        var currentVisibility = appWindow.visibility

        if (currentVisibility !== QWindow_Hidden)
        {
            var maximized = (appWindow.visibility === QWindow_Maximized)
            Devicesettings.setMainWindowMaximized(maximized)

            if (storeWindowSizeChanges && !maximized)
            {
                Devicesettings.setMainWindowWidth(width)
                Devicesettings.setMainWindowHeight(height)
            }
        }
    }

    Component.onCompleted: {
        Client.sync()
        heartbeat.start()
        scheduler.user = Client.userSettings
        scheduler.start()
        storeWindowSizeChanges = true

        if (showUserSettingsOnStartup())
        {
            showUserSettingsWindow()
        }
    }

    function showUserSettingsOnStartup() {
        return Client.userSettings.name.trim() === ""
    }

    function toggleTodoMode()
    {
        var newValue = !inbox.todoMode
        inbox.todoMode = newValue
        overlayTodoMode.active = newValue
        overlayTodoMode.show()
    }

    function toggleSoundActive()
    {
        Devicesettings.muted = !Devicesettings.muted
        var newValueSoundActive = !Devicesettings.muted
        overlaySoundActive.active = newValueSoundActive
        overlaySoundActive.show()
    }

    function toggleAnswer() {
        if (inbox.state == "reply") closeAnswer()
        else openAnswer()
    }

    function openAnswer() {
        rightColumn.buttonBoxState = "answerOpen"
        inbox.state = "reply"
        rightColumn.forceActiveFocus();
    }

    function closeAnswer() {
        rightColumn.buttonBoxState = "default"
        inbox.state = "basic"
        middleColumn.forceActiveFocus();
    }

    Scheduler {
        id: scheduler

        onShowBackupReminderRequested: {
            masterKeyBackupReminderWindow.openWindow()
        }
    }

    Timer {
        id: heartbeat
        interval: 10000
        repeat: true
        onTriggered: Client.sync()

        property bool needResync: false

        function syncAsap()
        {
            if (!Client.sync()) needResync = true
        }

        function resync()
        {
            console.debug("Starting resync ...")
            Client.sync()
            needResync = false
        }
    }

    ShortcutOverlay {
        id: shortcutOverlay
    }

    SettingsWindow {
        id: settingsWindow
        onAvatarChanged: rightColumn.refreshAvatar();
    }

    InfoWindow {
        id: infoWindow

        onDownloadUpdate: {
            closeWindow()
            updateDialog.openDialog()
            updateDialog.downloadUpdate()
        }
    }

    MasterKeyBackupReminderWindow {
        id: masterKeyBackupReminderWindow
    }

    Keys.onPressed: {
        if (SC.isCtrlAndKey(Qt.Key_N, event)) { event.accepted = true; inbox.startConversation()      }
        if (SC.isCtrlAndKey(Qt.Key_R, event)) { event.accepted = true; inbox.toggleAnswer()           }
        if (SC.isCtrlAndKey(Qt.Key_G, event)) { event.accepted = true; inbox.showUserSettingsWindow() }
        if (SC.isCtrlAndKey(Qt.Key_O, event)) { event.accepted = true; inbox.logout()                 }
        if (SC.isCtrlAndKey(Qt.Key_T, event)) { event.accepted = true; inbox.toggleTodoMode()         }
        if (!event.accepted)
        {
            if (event.key === Qt.Key_Question)
            {
                shortcutOverlay.fadeIn()
                event.accepted = true
            }
        }
    }

    SyncingBanner {
        id: syncingBanner
    }

    SyncErrorBanner {
        id: syncErrorBanner
    }

    ClientTooOldBanner {
        id: clientTooOldBanner
    }

    Timer {
        id: syncingBannerDelayTimer
        interval: 1000
        repeat: false
        triggeredOnStart: false
        onTriggered: {
            syncingBanner.show()
        }
    }

    Connections {
        target: Client
        onOpenConversation: {
            leftColumn.openConversation(conversationId)
        }
        onSyncStarted: {
            syncingBannerDelayTimer.restart()
            syncingBanner.progressText = ""
        }
        onSyncProgressend: {
            if (countMessagesTotal >= 10) {
                var percent = 100.0 * countMessagesProcessed/countMessagesTotal
                var progressText = Math.floor(percent) + "\u202F%";
                // U+202F: NARROW NO-BREAK SPACE.
                // Not necessarily supported, falls back to space on Ubuntu.

                syncingBanner.progressText = progressText
            }
        }
        onSyncFinished: {
            syncingBannerDelayTimer.stop()
            if (success) {
                syncErrorBanner.hide()
                clientTooOldBanner.hide()
            }
            syncingBanner.hide()
            if (heartbeat.needResync) heartbeat.resync()
            if (countMessagesNewUnread > 0) newMessageSound.play();
        }
        onSyncError: {
            var errmsg;
            if (error == SyncErrors.InternalServerError)
            {
                errmsg = qsTr("Server error. We're sorry :(")
            }
            else if (error == SyncErrors.NetworkError)
            {
                errmsg = qsTr("Couldn't connect to server. Are you online?")
            }
            else if (error == SyncErrors.Unauthorized)
            {
                errmsg = qsTr("Login failed.")
            }
            else if (error == SyncErrors.UnknownError)
            {
                errmsg = qsTr("Unknown error while syncing.")
            }
            syncErrorBanner.show(errmsg)
        }
        onClientTooOld: {
            clientTooOldBanner.show()
        }
        onDraftAttachmentsTooBig: {
            console.debug("Could not send message from conversation: " + conversationId + ". Creating dialog ...")

            var dialogMessageSize = size
            var dialogMessageSizeAllowed = sizeAllowed

            var openErrorDialog = function(obj) {
                var sizeHuman        = (dialogMessageSize        / 1024 / 1024).toLocaleString(Qt.locale(), 'f', 1)
                var sizeAllowedHuman = (dialogMessageSizeAllowed / 1024 / 1024).toLocaleString(Qt.locale(), 'f', 1)
                obj.title = qsTr("Message not sent")
                obj.text = qsTr("Message could not be sent because attachments are too big.") + " "
                        + qsTr("Attachments size: %1 MiB (allowed %2 MiB).").arg(sizeHuman).arg(sizeAllowedHuman) + " "
                        + qsTr("Please edit the message and sent again.")
                obj.open()
            }

            var component = Dynamic.createComponentOrThrow("/dialogs/ErrorDialog.qml");

            var incubator = component.incubateObject(parent);

            if (incubator.status !== Component.Ready)
            {
                incubator.onStatusChanged = function(status) {
                    if (status === Component.Ready)
                    {
                        console.debug("Object " + incubator.object + " is now ready!");
                        openErrorDialog(incubator.object);
                    }
                }
            }
            else
            {
                console.debug("Object " + incubator.object + " is ready immediately!");
                openErrorDialog(incubator.object);
            }
        }
    }

    GroupConversationStartDialog {
        id: startConversationPrompt
        text: qsTr("Enter Kullo address:")
        onAddressAccepted: {
            console.info("Start conversation with '" + result + "'")
            Client.addConversation(result)
        }
    }

    SoundEffect {
        id: newMessageSound
        source: "/resources/sounds/new_message.wav"
        volume: 0.5
        muted: Devicesettings.muted
    }

    states: [
        State {
            name: "basic"
            PropertyChanges {
                target: rightColumn
                width: 0
            }
        },
        State {
            name: "reply"
            PropertyChanges {
                target: rightColumn
                width: (inbox.width - leftColumn.width) / 2
            }
        }
    ]
    state: "basic"

    function startConversation() {
        startConversationPrompt.openDialog()
    }

    function showUserSettingsWindow() {
        settingsWindow.openWindow()
    }

    function showInfoWindow() {
        infoWindow.openWindow()
    }

    function logout() {
        Client.logOut()
        app.state = "welcome"
    }

    function openConversation(convId) {
        var conv = Client.conversations.get(convId)

        if (!conv) console.error("conv is null in inbox.openConversation() for conversation: " + convId);
        if (!conv.messages) console.error("conv.messages is null in inbox.openConversation() for conversation: " + convId);
        if (!conv.draft) console.error("conv.draft is null in inbox.openConversation() for conversation: " + convId);

        middleColumn.setModel(conv.messages)
        middleColumn.participantsAddresses = conv.participantsAddresses
        rightColumn.conversationId = convId

        conv.markAllMessagesAsRead()
    }

    FocusScope {
        id: inboxContent
        anchors.fill: parent
        focus: true

        LeftColumn {
            id: leftColumn
            anchors {
                bottom: parent.bottom
                left: parent.left
                top: parent.top
            }
            width: 230

            focus: true
            onActiveFocusChanged: {
                focusChanged(focus)
            }
            KeyNavigation.right: middleColumn
        }

        MiddleColumn {
            id: middleColumn

            anchors {
                left: leftColumn.right
                right: rightColumn.left
                bottom: parent.bottom
                top: parent.top
            }

            onActiveFocusChanged: {
                focusChanged(focus)
            }
            KeyNavigation.left: leftColumn
        }

        RightColumn {
            id: rightColumn

            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }

            onActiveFocusChanged: {
                focusChanged(focus)
            }
        }
    }

    Overlay {
        id: overlayTodoMode
        anchors.centerIn: parent
        text: qsTr("Todo\nmode");
    }

    Overlay {
        id: overlaySoundActive
        anchors.centerIn: parent
        text: qsTr("Sound\nactive");
    }
}
