/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
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
            InnerApplication.deviceSettings.setMainWindowMaximized(maximized)

            if (storeWindowSizeChanges && !maximized)
            {
                InnerApplication.deviceSettings.setMainWindowWidth(width)
                InnerApplication.deviceSettings.setMainWindowHeight(height)
            }
        }
    }

    Component.onCompleted: {
        Inbox.sync()
        heartbeat.start()
        scheduler.user = Inbox.userSettings
        scheduler.start()
        storeWindowSizeChanges = true
    }

    function userSettingsIncomplete() {
        return Inbox.userSettings.name.trim() === ""
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
        InnerApplication.deviceSettings.muted = !InnerApplication.deviceSettings.muted
        var newValueSoundActive = !InnerApplication.deviceSettings.muted
        overlaySoundActive.active = newValueSoundActive
        overlaySoundActive.show()
    }

    function toggleAnswer() {
        if (inbox.state == "reply") closeAnswer()
        else openAnswer()
    }

    function openAnswer() {
        rightColumn.buttonBoxState = "answerOpen"
        rightColumn.animationEnabled = true
        inbox.state = "reply"
        rightColumn.forceActiveFocus();
    }

    function closeAnswer() {
        rightColumn.buttonBoxState = "default"
        rightColumn.animationEnabled = true
        rightColumn.minimumWidthSet = false
        inbox.state = "basic"
        middleColumn.forceActiveFocus();
    }

    MessageDialog {
        id: openFileErrorDialog
        property string filename: ""
        title: qsTr("Error opening file")
        text: qsTr("File %1 could not be opened").arg(filename)
    }

    function showOpenFileError(filename) {
        openFileErrorDialog.filename = filename
        openFileErrorDialog.open()
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
        onTriggered: Inbox.sync()

        property bool needResync: false

        function syncAsap()
        {
            if (!Inbox.sync()) needResync = true
        }

        function resync()
        {
            console.debug("Starting resync ...")
            Inbox.sync()
            needResync = false
        }
    }

    ShortcutOverlay {
        id: shortcutOverlay
    }

    SettingsWindow {
        id: settingsWindow
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
        target: Inbox
        onOpenConversation: {
            leftColumn.openConversation(conversationId)
        }
        onSyncStarted: {
            syncingBannerDelayTimer.restart()
            syncingBanner.progressText = ""
        }
        onSyncProgressed: {
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
            if (userSettingsIncomplete()) showUserSettingsWindow()
        }
        onSyncError: {
            var errmsg;
            if (error == SyncErrors.ServerError)
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
            else
            {
                console.error("Unknown value for error: '" + error + "'")
            }
            syncErrorBanner.show(errmsg)
        }
        onClientTooOld: {
            clientTooOldBanner.show()
        }
        onDraftAttachmentsTooBig: {
            console.debug("Could not send message from conversation: " + conversationId + ". Creating dialog ...")

            var openErrorDialog = function(obj) {
                obj.title = qsTr("Message not sent")
                obj.text = qsTr("Message could not be sent because attachments are too big.") + " "
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
            Inbox.addConversation(result)
        }
    }

    AdvancedFileSaveDialog {
        id: globalSaveAttachmentDialog
        property var attachment: null // Qml::AttachmentModel

        title: qsTr("Save attachment")
        filename: attachment ? attachment.filename : ""

        onAccepted: {
            if (!attachment.saveTo(fileUrl))
            {
                console.error("Error while saving file: " + fileUrl);
            }
        }
    }

    FileDialog {
        id: globalSaveAttachmentsDialog
        property var attachments: null // Qml::AttachmentListModel

        selectFolder: true
        title: qsTr("Save all")
        folder: Utils.defaultSaveAttachmentsDir()

        onAccepted: {
            console.log("Save all attachments to " + fileUrl)
            attachments.saveAllTo(fileUrl)
        }
    }

    SoundEffect {
        id: newMessageSound
        source: "/resources/sounds/new_message.wav"
        volume: 0.5
        muted: InnerApplication.deviceSettings.muted
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
                width: InnerApplication.deviceSettings.answerColumnWidth
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
        Inbox.logOut()
        app.state = "welcome"
    }

    function openConversation(convId) {
        var conv = Inbox.conversations.get(convId)

        if (!conv) console.error("conv is null in inbox.openConversation() for conversation: " + convId);
        if (!conv.messages) console.error("conv.messages is null in inbox.openConversation() for conversation: " + convId);
        if (!conv.draft) console.error("conv.draft is null in inbox.openConversation() for conversation: " + convId);

        middleColumn.setModel(conv.messages)
        middleColumn.participantsAddresses = conv.participantsAddresses
        rightColumn.conversation = conv

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

        SplitView {
            id: splitView
            readonly property real handleWidth: 1 // we know that from the source code

            anchors {
                left: leftColumn.right
                right: parent.right
                bottom: parent.bottom
                top: parent.top
            }

            // when user resized the window
            onWidthChanged: rightColumn.updateWidthFromConstrains()

            MiddleColumn {
                id: middleColumn
                // 210 px is the width of one attachment delegate and all surrounding
                // paddings, margins and borders and the SYSTEM DEPENDENT 13 px scroll
                // bar on Ubuntu. As soon as we get below that, we'll crash.
                // Go for 210+something to deal with different scroll bar widths.
                readonly property real minimumWidth: 220

                Layout.fillWidth: true
                Layout.minimumWidth: minimumWidth // Avoids deep recursion crash from geometry updates

                onActiveFocusChanged: {
                    focusChanged(focus)
                }
                KeyNavigation.left: leftColumn
            }

            RightColumn {
                id: rightColumn
                property bool minimumWidthSet: false

                Layout.minimumWidth: minimumWidthSet ? 150 : 0
                visible: inbox.state == "reply" || width > 0

                function updateWidthFromConstrains() {
                    if (splitView.width < middleColumn.minimumWidth + splitView.handleWidth + width) {
                        width = splitView.width - (middleColumn.minimumWidth + splitView.handleWidth)
                    }
                }

                onWidthChanged: {
                    if (splitView.resizing) {
                        // user interaction
                        var widthToStore = Math.round(width)
                        widthToStore -= widthToStore%5
                        // console.debug(width + ", " + widthToStore)
                        InnerApplication.deviceSettings.answerColumnWidth = widthToStore
                        minimumWidthSet = true
                        animationEnabled = false
                    }
                }

                onActiveFocusChanged: {
                    focusChanged(focus)
                }
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
