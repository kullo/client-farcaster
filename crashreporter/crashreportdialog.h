/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <memory>
#include <QDialog>

#include "crashreporter/metadata.h"
#include "crashreporter/uploader.h"

namespace Ui {
class CrashReportDialog;
}

class CrashReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrashReportDialog(Uploader &uploader, const Metadata &meta_, QWidget *parent = 0);
    ~CrashReportDialog();

private slots:
    void onFinished(int result);
    void onUploadFinished(bool ok, QString errorMessage);

private:
    std::unique_ptr<Ui::CrashReportDialog> ui_;
    Metadata meta_;
    Uploader &uploader_;
};
