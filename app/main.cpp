/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QFontInfo>
#include <QIcon>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QRemoteObjectHost>
#include <QScopedPointer>
#include <QScreen>
#include <QSslSocket>
#include <QSysInfo>

#include <iostream>

#ifdef Q_OS_WIN
#    include <Windows.h>
#endif

#include "opentodolistqmlextensionsplugin.h"
#include "application.h"
#include "datastorage/cache.h"
#include "service/backgroundservice.h"

#ifdef OTL_USE_SINGLE_APPLICATION
#    include "singleapplication.h"
#else
#    include <QApplication>
#endif

#ifndef OPENTODOLIST_VERSION
#    include "../lib/opentodolist_version.h"
#endif
#include "utils/translations.h"

class AppStartup : public QObject
{
public:
    AppStartup();
    ~AppStartup() override;

    int exec(int& argc, char* argv[]);

private:
    QApplication* m_app;
    Cache* m_cache;
    QRemoteObjectHost* m_srcNode;
    BackgroundService* m_backgroundService;
    Application* m_application;

    QCommandLineParser m_parser;
    QQmlApplicationEngine* m_engine;
    OpenTodoList::Translations* m_translations;
    OpenTodoListQmlExtensionsPlugin m_qmlPlugin;

    void setupGlobals();
    void createApp(int& argc, char* argv[]);
    void setupFonts();
    void parseCommandLineArgs();
    void openConsole();
    void printDiagnostics();
    void createCache();
    void startBackgroundService();
    void startGUI();
};

int main(int argc, char* argv[])
{
    AppStartup appStartup;
    return appStartup.exec(argc, argv);
}

AppStartup::AppStartup()
    : m_app(nullptr),
      m_cache(nullptr),
      m_srcNode(nullptr),
      m_backgroundService(nullptr),
      m_application(nullptr),
      m_parser(),
      m_engine(nullptr),
      m_translations(nullptr),
      m_qmlPlugin()
{
    m_qmlPlugin.registerTypes("OpenTodoList");
}

AppStartup::~AppStartup() {}

void AppStartup::setupGlobals()
{
    QCoreApplication::setApplicationName("OpenTodoList");
    QCoreApplication::setApplicationVersion(OPENTODOLIST_VERSION);
    QCoreApplication::setOrganizationDomain("www.rpdev.net");
    QCoreApplication::setOrganizationName("RPdev");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#ifdef OPENTODOLIST_DEBUG
    QLoggingCategory(nullptr).setEnabled(QtDebugMsg, true);
#endif
}

void AppStartup::printDiagnostics()
{
    // Print diagnostic information
    qWarning() << "This is" << QCoreApplication::applicationName() << "version"
               << QCoreApplication::applicationVersion();
    qWarning() << "System ABI:" << QSysInfo::buildAbi();
    qWarning() << "Build CPU Architecture:" << QSysInfo::buildCpuArchitecture();
    qWarning() << "Current CPU Architecture:" << QSysInfo::currentCpuArchitecture();
    qWarning() << "Kernel Type:" << QSysInfo::kernelType();
    qWarning() << "Kernel Version:" << QSysInfo::kernelVersion();
    qWarning() << "Product Name:" << QSysInfo::prettyProductName();

    // Print SSL information for debugging
    qWarning() << "OpenSSL version Qt was built against:"
               << QSslSocket::sslLibraryBuildVersionString();
    qWarning() << "OpenSSL version loaded:" << QSslSocket::sslLibraryVersionString();
}

void AppStartup::createApp(int& argc, char* argv[])
{
#ifdef OTL_USE_SINGLE_APPLICATION
    m_app = new SingleApplication(argc, argv, false,
                                  SingleApplication::User | SingleApplication::ExcludeAppPath);
#else
    m_app = new QApplication app(argc, argv);
#endif
    m_app->setWindowIcon(QIcon(":/icons/hicolor/128x128/apps/net.rpdev.OpenTodoList.png"));
}

void AppStartup::setupFonts()
{
    // Load color emoji font:
    QFontDatabase::addApplicationFont(":/Fonts/NotoColorEmoji-unhinted/NotoColorEmoji.ttf");

#ifdef OPENTODOLIST_FLATPAK
    {
        QDir dir("/var/config/fontconfig/conf.d");
        if (dir.mkpath(".")) {
            QFile::copy("/app/etc/fonts/conf.d/90-otl-color-emoji.conf",
                        dir.absoluteFilePath("90-otl-color-emoji.conf"));
        }
    }
#endif
}

void AppStartup::parseCommandLineArgs()
{
    m_parser.setApplicationDescription(tr("Manage your personal data."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();

    // Enable touch screen optimizations
    QCommandLineOption enableTouchOption = { { "T", "enable-touch" },
                                             tr("Switch on some optimizations for touchscreens.") };
    m_parser.addOption(enableTouchOption);

#ifdef Q_OS_WIN
    m_parser.addOption(
            { "enable-console", tr("Enable a console on Windows to gather debug output") });
#endif

    m_parser.process(*m_app);
}

void AppStartup::openConsole()
{
#ifdef Q_OS_WIN
    if (m_parser.isSet("enable-console")) {
        FreeConsole();
        if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
            AllocConsole();
        }
    }
#endif
}

void AppStartup::createCache()
{
    m_cache = new Cache();
}

void AppStartup::startBackgroundService()
{
    m_srcNode = new QRemoteObjectHost(QUrl(QStringLiteral("local:opentodolist")));
    m_backgroundService = new BackgroundService(m_cache);
    m_srcNode->enableRemoting(m_backgroundService);
}

void AppStartup::startGUI()
{
    m_engine = new QQmlApplicationEngine;
    m_translations = new OpenTodoList::Translations(m_engine);
    QString qmlBase = "qrc:/";
    m_engine->addImportPath(qmlBase);

    m_application = new Application(m_cache);
    m_qmlPlugin.setApplication(m_application);
    m_qmlPlugin.registerTypes("OpenTodoList");

    m_engine->rootContext()->setContextProperty("application", m_app);
    m_engine->rootContext()->setContextProperty("applicationVersion",
                                                QVariant(OPENTODOLIST_VERSION));
    m_engine->rootContext()->setContextProperty("qmlBaseDirectory", qmlBase);
#ifdef OPENTODOLIST_DEBUG
    m_engine->rootContext()->setContextProperty("isDebugBuild", true);
#else
    m_engine->rootContext()->setContextProperty("isDebugBuild", false);
#endif

    auto url = QUrl(qmlBase + "main.qml");

    connect(
            m_engine, &QQmlApplicationEngine::objectCreated, m_app,
            [url](QObject* obj, const QUrl& objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            },
            Qt::QueuedConnection);

    m_engine->load(url);
}

int AppStartup::exec(int& argc, char* argv[])
{
    setupGlobals();
    createApp(argc, argv);
    setupFonts();
    parseCommandLineArgs();
    openConsole();
    printDiagnostics();
    createCache();
    startBackgroundService();
    startGUI();
    return m_app->exec();
}
