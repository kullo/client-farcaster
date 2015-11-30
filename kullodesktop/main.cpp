/* Copyright 2013–2015 Kullo GmbH. All rights reserved. */
#include <cstdlib>
#include <exception>
#include <sstream>

#include <QtQml>
#include <QQuickWindow>

#include <desktoputil/metatypes.h>
#include <desktoputil/qtypestreamers.h>

#include <apimirror/Client.h>

#include <kulloclient/api/Client.h>
#include <kulloclient/api/Registry.h>
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

#include "kullodesktop/osintegration/kullotrayicon.h"
#include "kullodesktop/osintegration/programoptions.h"
#include "kullodesktop/osintegration/singleinstanceerrorbox.h"
#include "kullodesktop/osintegration/singleinstancelock.h"

#include "kullodesktop/qml/clientmodel.h"
#include "kullodesktop/qml/fontlist.h"
#include "kullodesktop/qml/kulloversionchecker.h"
#include "kullodesktop/qml/modalitywatcher.h"
#include "kullodesktop/qml/os.h"
#include "kullodesktop/qml/softwareversions.h"
#include "kullodesktop/qml/utils.h"

#include "kullodesktop/util/breakpadsetup.h"
#include "kullodesktop/util/kullofoldersetup.h"
#include "kullodesktop/util/logengineconsoleextended.h"
#include "kullodesktop/util/logenginefilewriter.h"
#include "kullodesktop/util/logfilecleaner.h"
#include "kullodesktop/util/paths.h"
#include "kullodesktop/util/qdebugmessagehandler.h"
#include "kullodesktop/util/qmlsetup.h"

using namespace KulloDesktop;

int main(int argc, char *argv[])
{
    OsIntegration::ProgramOptions programOptions;
    int retParse = programOptions.parse(argc, argv);
    if (retParse != OsIntegration::ProgramOptions::NA) return retParse;

    int ret = programOptions.preApplicationActions();
    if (ret != OsIntegration::ProgramOptions::NA) return ret;

#ifdef QT_NO_DEBUG
    Util::LogEngineFileWriter::open();
    Kullo::Util::LibraryLogger::setLogFunction(&Util::LogEngineFileWriter::libraryLoggerWrapper);
#else
    if (programOptions.logFormatExtended_)
    {
        Kullo::Util::LibraryLogger::setLogFunction(&Util::LogEngineConsoleExtended::libraryLoggerWrapper);
    }
#endif
    Kullo::Util::LibraryLogger::setCurrentThreadName("main");

    qInstallMessageHandler(&Util::QDebugMessageHandler::handler);

    int execStatus;
    { // Lifespan of application
        #ifdef Q_OS_MAC
        Applications::OsxKulloApplication app(argc, argv);
        #else
        Applications::KulloApplication app(argc, argv);
        #endif

        // force use of ANGLE instead of OpenGL on Windows
        #ifdef Q_OS_WIN
        app.setAttribute(Qt::AA_UseOpenGLES, true);
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

        app.secondStageSetup();

        // this should be called as soon as possible, but after instantiation of a QCoreApplication
        Util::BreakpadSetup::setup(Applications::KulloApplication::TEST_MODE);

        programOptions.crashtestActions();

        /*
         * Translator
         */
        QString lang = QLocale::system().name();
        Log.i() << "System language: " << lang;
        QTranslator translator;
        translator.load(QStringLiteral(":/translations/farcaster_%1").arg(lang));
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
        OsIntegration::SingleInstanceLock lock(guid, Applications::KulloApplication::IGNORE_LOCK);

        if (!lock.acquire())
        {
            Log.e() << "Can't start more than one instance of the application.";
            OsIntegration::SingleInstanceErrorBox msgBox(app.applicationName());
            msgBox.exec();

            // Return from main() and clean everything properly
            // http://stackoverflow.com/questions/461449/return-statement-vs-exit-in-main
            return 1;
        }

        DesktopUtil::registerMetaTypes();

        Kullo::Http::Registry::setHttpClientFactory(
                    std::shared_ptr<Kullo::Http::HttpClientFactory>(
                        new HttpClient::HttpClientFactoryImpl()));

        auto tr = std::make_shared<Kullo::Util::StlTaskRunner>();
        Kullo::Api::Registry::setTaskRunner(tr);

        ApiMirror::Client client(Kullo::Api::Client::create());

        app.deviceSettings().migrate();

        Qml::FontList fontlist;
        Qml::Utils utils;
        Qml::SoftwareVersions softwareVersions;
        Qml::Os os;
        Qml::ModalityWatcher modalityWatcher;
        Qml::KulloVersionChecker kulloVersionChecker(app);

        /*
         * Create Client
         */
        Util::LogfileCleaner::clean();
        Util::KulloFolderSetup::perpareKulloFolder(Util::kulloPaths().KULLO_DIR);

        Qml::ClientModel clientModel(Util::kulloPaths().DATABASE_DIR + QStringLiteral("/libkulloclient-%1.db"), app);

        /*
         * Create GUI Engine
         */
        QQmlApplicationEngine engine;
        Util::QmlSetup::setupTypes();
        Util::QmlSetup::setupImageproviders(engine, clientModel);

        /*
         * Naming is hard.
         *
         * The following global QML objects get a capital letter in order
         * to not conflict with property names. Types of those objects get
         * a "Type" suffix, since this is not visible anywhere. E.g.
         *
         *   SomeOtherComponent {
         *       client: Client
         *   }
         *
         * where Client is of type ClientType.
         *
         */
        QQmlContext *context = engine.rootContext();
        context->setContextProperty("Client", &clientModel);
        // Rename to 'Client' as soon as Qml::ClientModel died.
        context->setContextProperty("ClientNewApi", &client);
        context->setContextProperty("KulloVersionChecker", &kulloVersionChecker);
        context->setContextProperty("Devicesettings", &app.deviceSettings());
        context->setContextProperty("FontList", &fontlist);
        context->setContextProperty("ModalityWatcher", &modalityWatcher);
        context->setContextProperty("Os", &os);
        context->setContextProperty("Utils", &utils);
        context->setContextProperty("SoftwareVersions", &softwareVersions);

        const QStringList INITIAL_QML_FILES {
            QStringLiteral("qrc:/libraryLoggerInitializer.qml"),
            QStringLiteral("qrc:/main.qml")
        };

        for (const auto url : INITIAL_QML_FILES)
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

        std::unique_ptr<OsIntegration::KulloTrayIcon> icon;
        if (!Applications::KulloApplication::NO_TRAY_ICON)
        {
            icon = Kullo::make_unique<OsIntegration::KulloTrayIcon>(app, *window, clientModel);
            Log.i() << "Done building tray icon.";
        }

        window->show();

        execStatus = app.exec();
        tr->wait();
        lock.release();
    }

    Log.i() << "Closing application with status " << execStatus;
    Util::LogEngineFileWriter::close();
    return execStatus;
}
