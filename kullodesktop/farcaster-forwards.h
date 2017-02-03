/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

namespace KulloDesktop {
    namespace Applications {
        class KulloApplication;
    }
    namespace OsIntegration {
        class KulloTrayIcon;
    }
    namespace Qml {
        class AttachmentModel;
        class ConversationListModel;
        class ConversationListSource;
        class ConversationModel;
        class DeviceSettings;
        class DraftAttachmentModel;
        class DraftAttachmentListModel;
        class DraftModel;
        class Inbox;
        class InnerApplication;
        class MessageListSource;
        class MessageModel;
        class Sender;
        class UserSettings;
    }
}

// Internals
namespace boost {
    namespace program_options {
        class variables_map;
        class options_description;
    }
}
