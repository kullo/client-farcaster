/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

namespace ApiMirror {
    namespace Enums {
        // We cannot forward declare inner classes like LocalErrorHolder::LocalError here:
        // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0289r0.pdf
        class DraftPartHolder;
        class LocalErrorHolder;
        class NetworkErrorHolder;
        class SyncPhaseHolder;
    }

    class Client;
    class ClientAddressExistsListener;
    class ClientCheckCredentialsListener;
    class ClientCreateSessionListener;
    class ClientGenerateKeysListener;
    class DraftAttachmentsAddListener;
    class DraftAttachmentsContentListener;
    class DraftAttachmentsSaveToListener;
    class EventDispatcher;
    class MessageAttachmentsContentListener;
    class MessageAttachmentsSaveToListener;
    class MessagesSearchListener;
    class RegistrationRegisterAccountListener;
    class SessionAccountInfoListener;
    class SessionListener;
    class SyncerListener;
}
