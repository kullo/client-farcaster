/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include <cstdlib>
#include <exception>
#include <signal.h>
#include <sstream>

#include <QtQml>
#include <QQuickWindow>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QSysInfo>

#include <desktoputil/breakpadsetup.h>
#include <desktoputil/metatypes.h>
#include <desktoputil/paths.h>
#include <desktoputil/qdebugmessagehandler.h>
#include <desktoputil/qtypestreamers.h>

#include <apimirror/Client.h>

#include <kulloclient/api/Client.h>
#include <kulloclient/api/Registry.h>
#include <kulloclient/api/Syncer.h>
#include <kulloclient/http/Registry.h>
#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/misc.h>
#include <kulloclient/util/stltaskrunner.h>
#include <httpclient/httpclientfactoryimpl.h>

#ifdef Q_OS_MAC
#include "kullodesktop/applications/osxkulloapplication.h"
#else
#include "kullodesktop/applications/kulloapplication.h"
#endif

#include "kullodesktop/osintegration/programoptions.h"
#include "kullodesktop/osintegration/singleinstancelock.h"

#include "kullodesktop/qml/fontlist.h"
#include "kullodesktop/qml/hdpi.h"
#include "kullodesktop/qml/inbox.h"
#include "kullodesktop/qml/innerapplication.h"
#include "kullodesktop/qml/kulloversionchecker.h"
#include "kullodesktop/qml/modalitywatcher.h"
#include "kullodesktop/qml/os.h"
#include "kullodesktop/qml/softwareversions.h"
#include "kullodesktop/qml/utils.h"
#include "kullodesktop/qml_bridge/qmlsetup.h"

#include "kullodesktop/util/consoleextendedloglistener.h"
#include "kullodesktop/util/htmlfileloglistener.h"
#include "kullodesktop/util/kullofoldersetup.h"
#include "kullodesktop/util/logfilecleaner.h"

using namespace KulloDesktop;

int main(int argc, char *argv[])
{
    OsIntegration::ProgramOptions programOptions;
    int retParse = programOptions.parse(argc, argv);
    if (retParse != OsIntegration::ProgramOptions::NA) return retParse;

    int ret = programOptions.preApplicationActions();
    if (ret != OsIntegration::ProgramOptions::NA) return ret;

#ifdef NDEBUG
    auto logListener = std::make_shared<Util::HtmlFileLogListener>();
    Kullo::Api::Registry::setLogListener(logListener);
#else
    if (programOptions.logFormatExtended_)
    {
        auto logListener = std::make_shared<Util::ConsoleExtendedLogListener>();
        Kullo::Api::Registry::setLogListener(logListener);
    }
#endif
    Kullo::Util::LibraryLogger::setCurrentThreadName("main");

    qInstallMessageHandler(&DesktopUtil::QDebugMessageHandler::handler);

    int execStatus;
    { // Lifespan of application
        #ifdef Q_OS_MAC
        Applications::OsxKulloApplication app(argc, argv);
        #else
        Applications::KulloApplication app(argc, argv);
        #endif

        app.setOrganizationName("Kullo");
        app.setOrganizationDomain("kullo.net");
        #ifdef QT_NO_DEBUG
        app.setApplicationName("Kullo");
        #else
        app.setApplicationName("KulloDebug");
        #endif

        int retPost = programOptions.postApplicationActions(app);
        if (retPost != OsIntegration::ProgramOptions::NA) return retPost;

        // this should be called as soon as possible, but after instantiation of a QCoreApplication
        DesktopUtil::BreakpadSetup::setup(Applications::KulloApplication::TEST_MODE);

        programOptions.crashtestActions();

        // Register SIGINT handler to cleanly close application on Ctrl+C
        // or `killall -SIGINT kullo`
        signal(SIGINT, [](int sig) ->void {
            Log.i() << "Quit the application (user request signal = " << sig << ")";
            Applications::KulloApplication::quit();
        });

        Log.i() << QSysInfo::buildCpuArchitecture() << " app "
                << "running on " << QSysInfo::currentCpuArchitecture() << "; "
                << "OS: " << QSysInfo::prettyProductName();

        /*
         * Translator
         */
        QLocale locale;
        Log.i() << "Locale: " << locale.name();
        QTranslator translator;
        translator.load(
                    locale,
                    QStringLiteral("farcaster"),       // filename
                    QStringLiteral("_"),               // prefix
                    QStringLiteral(":/translations")); // directory
        QApplication::installTranslator(&translator);

        /*
         * Single instance handling
         *
         * requires: KulloApplication, Translator
         */
        app.processEvents();
        #ifdef QT_NO_DEBUG
        const auto guid = Applications::KulloApplication::GUID;
        #else
        const auto guid = Applications::KulloApplication::GUID_DEBUG;
        #endif

        // lock must live longer than any DB accessing objects
        OsIntegration::SingleInstanceLock lock(guid);

        if (lock.acquire() == OsIntegration::SingleInstanceLock::AcquireResult::InformedOtherInstance)
        {
            // Return from main() and clean everything properly
            // http://stackoverflow.com/questions/461449/return-statement-vs-exit-in-main
            Log.i() << "Informed other kullo instance to come up.";
            return 5;
        }

        DesktopUtil::registerMetaTypes();

        auto httpClientFactory = Kullo::nn_make_shared<HttpClient::HttpClientFactoryImpl>();
        httpClientFactory->setAcceptLanguage(locale.bcp47Name().toStdString());
        Kullo::Http::Registry::setHttpClientFactory(httpClientFactory);

        auto taskRunner = Kullo::nn_make_shared<Kullo::Util::StlTaskRunner>();
        Kullo::Api::Registry::setTaskRunner(taskRunner);

        Qml::InnerApplication innerApplication(app);
        innerApplication.deviceSettings()->migrate();

        QObject::connect(&lock, &OsIntegration::SingleInstanceLock::showMainWindowRequested,
                         &innerApplication, &Qml::InnerApplication::showMainWindowIfPossible);

        ApiMirror::Client client(Kullo::Api::Client::create());

        /*
         * Create Client
         */
        Util::LogfileCleaner::clean();
        Util::KulloFolderSetup::perpareKulloFolder(DesktopUtil::kulloPaths().KULLO_DIR);

        Qml::Inbox inbox(innerApplication, client, taskRunner.get());

        /*
         * Create GUI Engine
         */
        QQmlApplicationEngine engine;
        QObject::connect(&engine, &QQmlApplicationEngine::quit, [] () {
            Log.f() << "Do not use QML's Qt.quit() because it is evil: "
                    << "it closes windows and may or may not quit the application, depending on magic signaling that does not match the documentation. "
                    << "Use InnerApplication.quit()";
        });

        QmlBridge::QmlSetup::setupTypes();
        QmlBridge::QmlSetup::setupImageproviders(engine, inbox);

        /*
         * Global object names must not conflict with the types in Util::QmlSetup::setupTypes()
         */
        QQmlContext *context = engine.rootContext();

        Qml::FontList fontlist;
        Qml::Hdpi hdpi;
        Qml::KulloVersionChecker kulloVersionChecker(innerApplication);
        Qml::ModalityWatcher modalityWatcher;
        Qml::Os os;
        Qml::SoftwareVersions softwareVersions;
        Qml::Utils utils;
        context->setContextProperty("InnerApplication", &innerApplication);
        context->setContextProperty("Inbox", &inbox);
        context->setContextProperty("FontList", &fontlist);
        context->setContextProperty("Hdpi", &hdpi);
        context->setContextProperty("KulloVersionChecker", &kulloVersionChecker);
        context->setContextProperty("ModalityWatcher", &modalityWatcher);
        context->setContextProperty("Os", &os);
        context->setContextProperty("SoftwareVersions", &softwareVersions);
        context->setContextProperty("Utils", &utils);

        const QStringList INITIAL_QML_FILES {
            QStringLiteral("qrc:/libraryLoggerInitializer.qml"),
            QStringLiteral("qrc:/main.qml")
        };

        for (const auto &url : INITIAL_QML_FILES)
        {
            engine.load(QUrl(url));
        }

        QList<QObject*> roots = engine.rootObjects();
        if (roots.size() != INITIAL_QML_FILES.size())
        {
            Log.f() << "Couldn't load all root objects of " << INITIAL_QML_FILES;
            return -1;
        }
        QObject *topLevel = roots[1];

        QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
        if (!window) {
            Log.f() << "Error: Your root item has to be a window.";
            return -1;
        }

        // this connection must be established before show() is called
        QObject::connect(window, &QQuickWindow::sceneGraphInitialized,
                         [=] () -> void {
            auto context = window->openglContext();
            auto functions = context->functions();
            const std::string vendor   = reinterpret_cast<const char*>(functions->glGetString(GL_VENDOR));
            const std::string renderer = reinterpret_cast<const char*>(functions->glGetString(GL_RENDERER));
            const std::string version  = reinterpret_cast<const char*>(functions->glGetString(GL_VERSION));
            Log.i() << "OpenGL vendor: " << vendor << " "
                    << "renderer: " << renderer << " "
                    << "version: " << version;
        });

        window->show();
        innerApplication.setMainWindow(window);

        execStatus = app.exec();

        if (inbox.session())
        {
            Log.d() << "Canceling sync tasks ...";
            inbox.session()->syncer()->cancel();
        }

        taskRunner->wait();
        lock.release();
    }

    Log.i() << "Closing application with status " << execStatus;
    Kullo::Api::Registry::setLogListener(nullptr);

    // Uninstall LibraryLogger handler to avoid crashing when Qt creates log messages
    // during static destruction, e.g. https://bugreports.qt.io/browse/QTBUG-66910
    qInstallMessageHandler(nullptr);

    return execStatus;
}
