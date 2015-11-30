/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include "singleinstanceerrorbox.h"

namespace KulloDesktop {
namespace OsIntegration {

SingleInstanceErrorBox::SingleInstanceErrorBox(const QString &appName, QWidget *parent)
    : QMessageBox(parent)
{
    setIcon(QMessageBox::Critical);

    const auto line1 = tr("Can't start more than one instance of %1.").arg(appName);
    const auto line2 = tr("Is Kullo running in background?");
    const auto line3 = tr("If Kullo crashed before, you might need to restart your computer.");

    setText(line1 + " " + line2 + " " + line3);
}

}
}
