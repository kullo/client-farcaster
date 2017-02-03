/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include <iostream>
#include <QApplication>
#include <QCommandLineParser>

#include <kulloclient/registry.h>
#include <kulloclient/http/Registry.h>
#include <httpclient/httpclientfactoryimpl.h>

#include "crashreporter/crashreportdialog.h"
#include "crashreporter/metadata.h"

static Metadata getCommandLineArgs(QApplication &app, QCommandLineParser &parser)
{
    parser.addHelpOption();
    QCommandLineOption uploadUrlOption("upload_url",
                                       "Upload URL",
                                       "url");
    parser.addOption(uploadUrlOption);
    QCommandLineOption productNameOption("product_name",
                                         "Product name",
                                         "name");
    parser.addOption(productNameOption);
    QCommandLineOption productVersionOption("product_version",
                                            "Product version",
                                            "version");
    parser.addOption(productVersionOption);
    QCommandLineOption clientGuidOption("client_guid",
                                        "Unique ID of the client",
                                        "guid");
    parser.addOption(clientGuidOption);
    QCommandLineOption dumpPathOption("dump_path",
                                      "Path of the dump file",
                                      "path");
    parser.addOption(dumpPathOption);
    QCommandLineOption processUptimeOption("process_uptime",
                                           "Time since process start (optional)",
                                           "ms");
    parser.addOption(processUptimeOption);
    QCommandLineOption cumulativeUptimeOption("cumulative_uptime",
                                              "Cumulative running time since install (optional)",
                                              "ms");
    parser.addOption(cumulativeUptimeOption);
    QCommandLineOption emailOption("email",
                                   "Email address of user (optional)",
                                   "address");
    parser.addOption(emailOption);
    QCommandLineOption commentsOption("comments",
                                      "User comments (optional)",
                                      "comments");
    parser.addOption(commentsOption);

    parser.process(app);

    Metadata meta;
    meta.uploadUrl = parser.value(uploadUrlOption).toStdString();
    meta.productName = parser.value(productNameOption).toStdString();
    meta.productVersion = parser.value(productVersionOption).toStdString();
    meta.clientGuid = parser.value(clientGuidOption).toStdString();
    meta.dumpPath = parser.value(dumpPathOption).toStdString();
    meta.processUptime = parser.value(processUptimeOption).toStdString();
    meta.cumulativeUptime = parser.value(cumulativeUptimeOption).toStdString();
    meta.email = parser.value(emailOption).toStdString();
    meta.comments = parser.value(commentsOption).toStdString();
    return meta;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Kullo Crash Reporter");
    app.setQuitOnLastWindowClosed(false);

    Kullo::Util::LibraryLogger::setCurrentThreadName("main");

    Kullo::Http::Registry::setHttpClientFactory(
                std::shared_ptr<Kullo::Http::HttpClientFactory>(
                    new HttpClient::HttpClientFactoryImpl()));

    QCommandLineParser parser;
    Metadata meta = getCommandLineArgs(app, parser);
    if (!meta.valid())
    {
        std::cerr << parser.helpText().toStdString() << std::endl;
        return 1;
    }

    Uploader uploader;
    CrashReportDialog w(uploader, meta);
    w.show();

    return app.exec();
}
