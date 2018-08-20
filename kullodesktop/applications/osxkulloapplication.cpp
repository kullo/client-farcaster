/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "osxkulloapplication.h"

#include <kulloclient/util/assert.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>

#ifdef Q_OS_MAC
#include <objc/objc.h>
#include <objc/message.h>
#endif

namespace KulloDesktop {
namespace Applications {

#ifdef Q_OS_MAC
namespace {
bool dockClickHandler(id self, SEL _cmd, ...)
{
    K_UNUSED(self);
    K_UNUSED(_cmd);

    auto app = static_cast<OsxKulloApplication *>(QCoreApplication::instance());
    kulloAssert(app);
    app->onDockIconClicked();

    return false; // Suppress OS X defaults
}
}
#endif

OsxKulloApplication::OsxKulloApplication(int &argc, char **argv)
    : KulloApplication(argc, argv)
{
#ifdef Q_OS_MAC
    Class cls = objc_getClass("NSApplication");
    objc_object *appInst = objc_msgSend((objc_object*)cls, sel_registerName("sharedApplication"));
    kulloAssert(appInst);

    objc_object* delegate = objc_msgSend(appInst, sel_registerName("delegate"));
    kulloAssert(delegate);

    Class delClass = (Class)objc_msgSend(delegate,  sel_registerName("class"));
    SEL shouldHandle = sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:");
    if (class_getInstanceMethod(delClass, shouldHandle))
    {
        if (class_replaceMethod(delClass, shouldHandle, (IMP)dockClickHandler, "B@:"))
            Log.d() << "Registered dock click handler (replaced original method)";
        else
            Log.w() << "Failed to replace method for dock click handler";
    }
    else
    {
        if (class_addMethod(delClass, shouldHandle, (IMP)dockClickHandler,"B@:"))
            Log.d() << "Registered dock click handler";
        else
            Log.w() << "Failed to register dock click handler";
    }
#endif
}

void OsxKulloApplication::onDockIconClicked()
{
    emit showMainWindowRequested();
}

}
}
