/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtMultimedia 5.0
import Kullo 1.0

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "inbox"
import "inbox/banners"
import "buttons"
import "dialogs"
import "misc"
import "native"
import "search"
import "usersettings"
import "windows"
import "js/dynamic.js" as Dynamic
import "js/format.js" as Format
import "js/shortcut.js" as SC

FocusScope {
    id: inboxScreen

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

    function toggleTodoMode()
    {
        var newValue = !inboxScreen.todoMode
        inboxScreen.todoMode = newValue
        toastTodoMode.active = newValue
        toastTodoMode.show()
    }

    function toggleAnswer() {
        if (inboxScreen.state == "reply") closeAnswer()
        else openAnswer()
    }

    function openAnswer() {
        rightColumn.buttonBoxState = "answerOpen"
        rightColumn.animationEnabled = true
        inboxScreen.state = "reply"
        rightColumn.forceActiveFocus()
    }

    function closeAnswer() {
        rightColumn.buttonBoxState = "default"
        rightColumn.animationEnabled = true
        rightColumn.minimumWidthSet = false
        inboxScreen.state = "basic"
        middleColumn.forceActiveFocus()
    }

    function openSearch(searchText, conversationId, global) {
        searchText = typeof searchText !== 'undefined' ? searchText : "";
        conversationId = typeof conversationId !== 'undefined'
                ? conversationId
                : middleColumn.conversation.id;
        global = typeof global !== 'undefined' ? global : true;

        searchOverlay.active = true
        searchOverlay.item.searchText = searchText
        searchOverlay.item.conversationId = conversationId
        searchOverlay.item.global = global
        searchOverlay.item.direction = "both"
        searchOverlay.item.updateResults()
        searchOverlay.item.fadeIn()
    }

    function openSaveAttachmentDialog(attachment) {
        globalSaveAttachmentDialog.attachment = attachment
        globalSaveAttachmentDialog.openDialog()
    }

    function openSaveAttachmentsDialog(attachments) {
        globalSaveAttachmentsDialog.attachments = attachments
        globalSaveAttachmentsDialog.open()
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

    StableTimer {
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

    ProfileOverlay {
        id: profileOverlay
    }

    Loader {
        id: searchOverlay
        active: false
        source: "/search/SearchOverlay.qml"
        anchors.fill: parent
        z: 4
        onLoaded: {
            item.contentWidth = Qt.binding(function() {
                return Math.min(0.92*inboxScreen.width, 1100 * Hdpi.FontScalingFactor)
            })
            item.contentHeight = Qt.binding(function() {
                return inboxScreen.height - 40 * Hdpi.FontScalingFactor
            })
        }
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
        if (SC.isCtrlAndKey(Qt.Key_F, event)) { event.accepted = true; inboxScreen.openSearch()             }
        if (SC.isCtrlAndKey(Qt.Key_N, event)) { event.accepted = true; inboxScreen.startConversation()      }
        if (SC.isCtrlAndKey(Qt.Key_R, event)) { event.accepted = true; inboxScreen.toggleAnswer()           }
        if (SC.isCtrlAndKey(Qt.Key_G, event)) { event.accepted = true; inboxScreen.showUserSettingsWindow() }
        if (SC.isCtrlAndKey(Qt.Key_O, event)) { event.accepted = true; inboxScreen.closeSession()           }
        if (SC.isCtrlAndKey(Qt.Key_T, event)) { event.accepted = true; inboxScreen.toggleTodoMode()         }
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

    function resetSyncErrorBanners() {
        syncErrorBanner.hide()
        clientTooOldBanner.hide()
    }

    StableTimer {
        id: syncingBannerDelayTimer
        // first syncer is slow at the moment
        interval: 750
        repeat: false
        running: false
        onTriggered: {
            syncingBanner.show()
        }
    }

    Connections {
        target: Inbox
        property bool _firstSuccessfulSync: true
        onOpenConversation: {
            openConversation(conversationId)
        }
        onSyncStarted: {
            syncingBannerDelayTimer.restart()
            syncingBanner.reset()
        }
        onSyncProgressed: {
            var anythingHappened = incomingMessagesProcessed > 0
                    || incomingAttachmentsDownloadedBytes > 0
                    || outgoingMessagesUploadedBytes > 0
            if (anythingHappened) {
                resetSyncErrorBanners()
            }

            if (phase === SyncPhase.IncomingMessages
                    && incomingMessagesTotal >= 5) {
                syncingBanner.text = qsTr("Syncing messages")
                var processRatio = incomingMessagesTotal > 0
                    ? incomingMessagesProcessed/incomingMessagesTotal
                    : 0
                syncingBanner.progressIndeterminate = processRatio < 0.1
                syncingBanner.progressValue = processRatio
            } else if (phase === SyncPhase.OutgoingMessages
                       && outgoingMessagesTotalBytes > 0) {
                syncingBanner.text = qsTr("Sending")
                var uploadRatio = outgoingMessagesTotalBytes > 0
                    ? outgoingMessagesUploadedBytes/outgoingMessagesTotalBytes
                    : 0
                syncingBanner.progressIndeterminate = uploadRatio < 0.1
                syncingBanner.progressValue = uploadRatio
            } else if (phase === SyncPhase.IncomingAttachments
                       && incomingAttachmentsTotalBytes > 0) {
                syncingBanner.text = qsTr("Downloading attachments")
                var downloadRatio = incomingAttachmentsTotalBytes > 0
                        ? incomingAttachmentsDownloadedBytes/incomingAttachmentsTotalBytes
                        : 0
                syncingBanner.progressIndeterminate = downloadRatio < 0.1
                syncingBanner.progressValue = downloadRatio
            } else {
                syncingBanner.text = qsTr("Syncing")
                syncingBanner.progressIndeterminate = true
            }
        }
        onSyncPhaseChanged: {
            syncingBannerDelayTimer.restart()
        }
        onSyncFinished: {
            syncingBannerDelayTimer.stop()
            if (success) {
                resetSyncErrorBanners()
            }
            syncingBanner.hide()
            if (heartbeat.needResync) heartbeat.resync()
            if (countMessagesNewUnread > 0) newMessageSound.play()

            // when closing the session this slot might still be running when there is no user anymore
            if (Inbox.userSettings && _firstSuccessfulSync)
            {
                if (Inbox.userSettings.name.trim() === "")
                {
                    // this probably means that user is coming from registration
                    var localPart = Inbox.userSettings.address.split("#")[0]
                    Inbox.userSettings.name = localPart
                    profileOverlay.fadeIn()
                }
            }

            _firstSuccessfulSync = false
        }
        onSyncError: {
            var errmsg

            switch(error) {
            case NetworkError.Server:
                errmsg = qsTr("Server error. We're sorry :(")
                break;
            case NetworkError.Connection:
                errmsg = qsTr("Couldn't connect to server. Are you online?")
                break;
            case NetworkError.Unauthorized:
                errmsg = qsTr("Login failed.")
                break;
            case NetworkError.Unknown:
                errmsg = qsTr("Unknown error while syncing.")
                break;
            default:
                console.error("Unhandled value for error: '" + error + "'")
            }

            if (errmsg) syncErrorBanner.show(errmsg)
        }
        onClientTooOld: {
            clientTooOldBanner.show()
        }
        onDraftPartTooBig: {
            console.debug("Could not send message from conversation: " + conversationId + ". Creating dialog ...")

            var openErrorDialog = function(obj) {
                obj.title = qsTr("Message not sent")
                switch (part) {
                case DraftPart.Content:
                    obj.text = qsTr("Message could not be sent because the text is too long.")
                    break;
                case DraftPart.Attachments:
                    obj.text = qsTr("Message could not be sent because attachments are too big.")
                    break;
                default:
                    obj.text = ""
                    console.error("Unknown draft part: " + part)
                }
                obj.text +=
                        " " + qsTr("Current size") + ": " + Format.filesize_human(currentSize) + "," +
                        " " + qsTr("size limit") + ": " + Format.filesize_human(maxSize) + "." +
                        " " + qsTr("Please edit the message and send again.")
                obj.open()
            }

            var component = Dynamic.createComponentOrThrow("/dialogs/ErrorDialog.qml")
            var incubator = component.incubateObject(parent)
            if (incubator.status !== Component.Ready)
            {
                incubator.onStatusChanged = function(status) {
                    if (status === Component.Ready)
                    {
                        console.debug("Object " + incubator.object + " is now ready!")
                        openErrorDialog(incubator.object)
                    }
                }
            }
            else
            {
                console.debug("Object " + incubator.object + " is ready immediately!")
                openErrorDialog(incubator.object)
            }
        }
    }

    ConversationInfoWindow {
        id: conversationInfoWindow

        onStartConversationWithParticipants: {
            startConversationPrompt.openDialog()
            participants.sort()
            startConversationPrompt.addAddresses(participants)
        }
    }

    StartConversationDialog {
        id: startConversationPrompt
        title: qsTr("Start conversation")
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
            if (!attachment.saveToAsync(fileUrl))
            {
                console.error("Error while saving file: " + fileUrl)
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
            attachments.saveAllToAsync(fileUrl)
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

    function closeSession() {
        Inbox.closeSession()
        app.state = "welcome"
    }

    function openConversation(convId) {
        var conv = Inbox.visibleConversations.get(convId)

        if (!conv) console.error("conv is null in inbox.openConversation() for conversation: " + convId)
        if (!conv.messages) console.error("conv.messages is null in inbox.openConversation() for conversation: " + convId)
        if (!conv.draft) console.error("conv.draft is null in inbox.openConversation() for conversation: " + convId)

        leftColumn.selectConversation(convId)
        middleColumn.conversation = conv
        rightColumn.conversation = conv

        conv.markAllMessagesAsRead()
    }

    function selectMessage(messageId) {
        middleColumn.selectMessage(messageId)
    }

    FocusScope {
        id: inboxContent
        anchors.fill: parent
        focus: true

        PopupMenu {
            id: popupMenu

            property real placeAtX
            property real placeAtY

            function placeAt(target) {
                // place at full pixels to avoid placing elements inside of PopupMenu to pixel fractions
                placeAtX = Math.round(target.x - (width/2))
                placeAtY = Math.round(target.y - height)
                x = placeAtX
            }

            function open() {
                forceActiveFocus()
            }

            function close() {
                focus = false
            }

            property bool opened: focus

            opacity: 0 // init invisible
            visible: opacity != 0 // make sure the element is not clickable after closing

            states: [
                State {
                    when: popupMenu.opened
                    PropertyChanges { target: popupMenu; opacity: 1.0; y: placeAtY }
                },
                State {
                    when: !popupMenu.opened
                    PropertyChanges { target: popupMenu; opacity: 0.0; y: placeAtY-10 }
                }
            ]

            transitions: [
                Transition {
                    NumberAnimation { properties: "opacity,y"; duration: 100 }
                }
            ]

            NativeButton {
                text: qsTr("My Kullo")
                tooltip: qsTr("Show profile")
                onClicked: {
                    popupMenu.close()
                    profileOverlay.fadeIn()
                }

                width: parent.width-2*parent.padding
                style: KulloButtonStyle {
                    iconPosition: _POSITION_LEFT
                    paddingH: 10 * Hdpi.FontScalingFactor
                    source: "/resources/scalable/profile_w.svg"
                    backgroundColor: "transparent"
                    hoverColor: "#33ffffff"
                    textColor: Style.white
                }
            }

            NativeButton {
                text: qsTr("Settings")
                tooltip: qsTr("Show settings")
                         + " (%1)".arg(SC.nameOfCtrlAndKey("G", Os.osx))
                onClicked: {
                    popupMenu.close()
                    inboxScreen.showUserSettingsWindow()
                }

                width: parent.width-2*parent.padding
                style: KulloButtonStyle {
                    iconPosition: _POSITION_LEFT
                    paddingH: 10 * Hdpi.FontScalingFactor
                    source: "/resources/scalable/settings2_w.svg"
                    backgroundColor: "transparent"
                    hoverColor: "#33ffffff"
                    textColor: Style.white
                }
            }

            NativeButton {
                text: qsTr("Info")
                tooltip: qsTr("Show info window")
                onClicked: {
                    popupMenu.close()
                    inboxScreen.showInfoWindow()
                }

                width: parent.width-2*parent.padding
                style: KulloButtonStyle {
                    iconPosition: _POSITION_LEFT
                    paddingH: 10 * Hdpi.FontScalingFactor
                    source: "/resources/scalable/info_w.svg"
                    backgroundColor: "transparent"
                    hoverColor: "#33ffffff"
                    textColor: Style.white
                    badgeEnabled: KulloVersionChecker.updateAvailable
                }
            }

            NativeButton {
                text: qsTr("Leave inbox")
                tooltip: qsTr("Go to start screen")
                         + " (%1)".arg(SC.nameOfCtrlAndKey("O", Os.osx))
                onClicked: {
                    popupMenu.close()
                    inboxScreen.closeSession()
                }

                width: parent.width-2*parent.padding
                style: KulloButtonStyle {
                    iconPosition: _POSITION_LEFT
                    paddingH: 10 * Hdpi.FontScalingFactor
                    source: "/resources/scalable/logout2_w.svg"
                    backgroundColor: "transparent"
                    hoverColor: "#33ffffff"
                    textColor: Style.white
                }
            }
        }

        LeftColumn {
            id: leftColumn
            anchors {
                bottom: parent.bottom
                left: parent.left
                top: parent.top
            }
            width: 230 * Hdpi.FontScalingFactor

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
                readonly property real minimumWidth: 220 * Hdpi.FontScalingFactor

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

                Layout.minimumWidth: minimumWidthSet ? 150 * Hdpi.FontScalingFactor : 0
                visible: inboxScreen.state == "reply" || width > 0

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

    Toast {
        id: toastTodoMode
        anchors.centerIn: parent
        text: qsTr("Todo\nmode")
    }
}
