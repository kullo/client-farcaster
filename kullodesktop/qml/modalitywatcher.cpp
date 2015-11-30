/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "modalitywatcher.h"

#include <kulloclient/util/librarylogger.h>
#include <desktoputil/qtypestreamers.h>

namespace KulloDesktop {
namespace Qml {

ModalityWatcher::ModalityWatcher(QObject *parent)
    : QObject(parent)
{
}

bool ModalityWatcher::aboutToOpen(QString objectName)
{
    if (objectName.trimmed().isEmpty())
    {
        Log.w() << "objectName sould not be empty if window should be opened";
    }
    else
    {
        Log.d() << "Object about to open: " << objectName;
    }

    if (currentObject_)
    {
        Log.e() << "Modality currently blocked by: " << *currentObject_;
        return false;
    }
    else
    {
        currentObject_ = objectName;
        return true;
    }
}

void ModalityWatcher::closed(QString objectName)
{
    Log.d() << "Closed: " << objectName;

    if (!currentObject_)
    {
        Log.e() << "Closing object but non has been opened.";
        return;
    }

    if (*currentObject_ != objectName)
    {
        Log.w() << "Stored object name " << *currentObject_
                << " is not closed object name " << objectName;
    }

    currentObject_ = boost::none;
}

}
}
