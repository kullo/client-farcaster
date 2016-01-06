/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "crashreportdialog.h"

#include <iostream>
#include <kulloclient/util/librarylogger.h>

#include "ui_crashreportdialog.h"

CrashReportDialog::CrashReportDialog(Uploader &uploader, const Metadata &meta, QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::CrashReportDialog),
    meta_(meta),
    uploader_(uploader)
{
    ui_->setupUi(this);
    connect(&uploader_, &Uploader::finished,
            this, &CrashReportDialog::onUploadFinished);
    connect(this, &CrashReportDialog::finished,
            this, &CrashReportDialog::onFinished);
}

CrashReportDialog::~CrashReportDialog()
{
}

void CrashReportDialog::onFinished(int result)
{
    if (result != QDialog::Accepted)
    {
        QApplication::quit();
        return;
    }

    meta_.comments = ui_->commentTextEdit->toPlainText().toStdString();
    meta_.email = ui_->emailLineEdit->text().toStdString();

    uploader_.upload(meta_);
}

void CrashReportDialog::onUploadFinished(bool ok, QString errorMessage)
{
    if (ok)
    {
        Log.i() << "Upload complete.";
        QApplication::exit(0);
    }
    else
    {
        Log.e() << "Upload failed. Reason: " << errorMessage.toStdString();
        QApplication::exit(1);
    }
}
