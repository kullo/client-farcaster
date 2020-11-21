/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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
        class ConversationListSorted;
        class ConversationListSortedFiltered;
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
