/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "exceptionawareqapplication.h"

#include <exception>

#include <private/qobject_p.h>
#include <private/qmetaobject_p.h>
#include <QMetaMethod>

#include <desktoputil/qtypestreamers.h>
#include <kulloclient/util/exceptions.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/stacktrace.h>

namespace KulloDesktop {
namespace Applications {

ExceptionAwareQApplication::ExceptionAwareQApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
}

bool ExceptionAwareQApplication::notify(QObject *receiver, QEvent *event)
{
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch (std::exception &ex)
    {
        // Type QMetaCallEvent requires private header <private/qobject_p.h>
        QMetaCallEvent *metaCallEvent = dynamic_cast<QMetaCallEvent *>(event);
        if (metaCallEvent)
        {
            QString receiverClassName = receiver->metaObject()->className();

            QString signalSignature;
            const auto sender = metaCallEvent->sender();
            const auto senderMeta = sender->metaObject();

            for (auto i = senderMeta->methodOffset(); i < senderMeta->methodCount(); ++i)
            {
                const auto method = senderMeta->method(i);
                if (QMetaObjectPrivate::signalIndex(method) == metaCallEvent->signalId())
                {
                    signalSignature = QString::fromUtf8(method.methodSignature());
                }
            }

            QString senderClassName = senderMeta->className();

            Log.e() << "Caught std::exception during Qt slot invocation in class "
                    << receiverClassName
                    << " after emitting signal " << signalSignature
                    << " from sender " << senderClassName;
        }

        Kullo::Util::Stacktrace::toStdErr();
        Log.f() << Kullo::Util::formatException(ex).c_str();
        return false;
    }
}

}
}
