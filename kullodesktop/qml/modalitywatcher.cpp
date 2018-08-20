/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
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

    if (currentObjectName_)
    {
        Log.e() << "Modality currently blocked by: " << *currentObjectName_;
        return false;
    }
    else
    {
        currentObjectName_ = objectName;
        return true;
    }
}

void ModalityWatcher::closed(QString objectName)
{
    Log.d() << "Closed: " << objectName;

    if (!currentObjectName_)
    {
        Log.e() << "Closing object but non has been opened.";
        return;
    }

    if (*currentObjectName_ != objectName)
    {
        Log.w() << "Stored object name " << *currentObjectName_
                << " is not closed object name " << objectName;
    }

    currentObjectName_ = boost::none;
}

}
}
