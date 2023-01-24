/*
 * Copyright 2021 Martin Hoeher <martin@rpdev.net>
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

#include "appstartup.h"

#ifdef Q_OS_ANDROID
#    include <private/qandroidextras_p.h>
#    include <QStandardPaths>
#endif

#ifdef Q_OS_IOS
#    include <TargetConditionals.h>
#    if TARGET_OS_SIMULATOR
#        include <QTcpServer>
#    endif
#endif

static Q_LOGGING_CATEGORY(log, "OpenTodoList.main", QtDebugMsg);

QVector<QtMessageHandler> AppStartup::s_prevMessageHandler;

AppStartup::AppStartup()
    : m_app(nullptr),
      m_cache(nullptr),
      m_srcNode(nullptr),
      m_backgroundService(nullptr),
      m_application(nullptr),
#ifdef OPENTODOLIST_WITH_KNOTIFICATIONS
      m_statusNotifierItem(nullptr),
#else
      m_trayIcon(nullptr),
#endif
      m_trayMenu(nullptr),
#ifdef Q_OS_MACOS
      m_menuBar(nullptr),
#endif
      m_parser(),
      m_engine(nullptr),
      m_translations(nullptr),
      m_qmlPlugin()
{
    m_qmlPlugin.registerTypes("OpenTodoList");
}

AppStartup::~AppStartup()
{
#ifdef OPENTODOLIST_WITH_KNOTIFICATIONS
    delete m_statusNotifierItem;
    m_statusNotifierItem = nullptr;
#else
    delete m_trayIcon;
    m_trayIcon = nullptr;
#endif
    delete m_trayMenu;
    m_trayMenu = nullptr;
    delete m_application;
    m_application = nullptr;
    delete m_srcNode;
    m_backgroundService = nullptr;
    delete m_backgroundService;
    m_backgroundService = nullptr;
    delete m_cache;
    m_cache = nullptr;
#ifdef Q_OS_MACOS
    delete m_menuBar;
#endif
    delete m_app;
    m_app = nullptr;
}

void AppStartup::setupGlobals()
{
    QCoreApplication::setApplicationName("OpenTodoList");
    QCoreApplication::setApplicationVersion(OPENTODOLIST_VERSION);
    QCoreApplication::setOrganizationDomain("www.rpdev.net");
    QCoreApplication::setOrganizationName("RPdev");

    // Workaround for https://bugreports.qt.io/browse/QTBUG-109369:
    // Switch back to old config file location on Android:
#ifdef Q_OS_ANDROID
    {
        auto home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, home + "/" + ".config");
        QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, home + "/" + ".config");
    }
#endif

    // Apply user settings:
    {
        QSettings settings;
        settings.beginGroup("ApplicationWindow");
        if (settings.value("useDenseVariant", false).toBool()) {
            qputenv("QT_QUICK_CONTROLS_MATERIAL_VARIANT", "Dense");
        } else {
            qputenv("QT_QUICK_CONTROLS_MATERIAL_VARIANT", "Normal");
        }

        if (settings.value("overrideUiScaling", false).toBool()) {
            auto scale = settings.value("uiScaling", 100).toInt() / 100.0;
            qputenv("QT_SCALE_FACTOR", QByteArray::number(scale));
        }
    }
    qunsetenv("QT_AUTO_SCREEN_SCALE_FACTOR");

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

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
#ifdef Q_OS_ANDROID
    for (int i = 0; i < argc; ++i) {
        QByteArray opt(argv[i]);
        if (opt == "--service") {
            qWarning() << "Is service, starting QAndroidService";
            m_app = new QAndroidService(argc, argv);
            return;
        }
    }
#endif
#ifdef OTL_USE_SINGLE_APPLICATION
    m_app = new SingleApplication(argc, argv, true,
                                  SingleApplication::User | SingleApplication::ExcludeAppPath
                                          | SingleApplication::SecondaryNotification);
#else
    m_app = new QApplication(argc, argv);
#endif
    auto guiApp = qobject_cast<QGuiApplication*>(m_app);
    if (guiApp) {
        guiApp->setWindowIcon(QIcon(":/icons/hicolor/128x128/apps/net.rpdev.OpenTodoList.png"));
        guiApp->setDesktopFileName("net.rpdev.OpenTodoList");
    }

#ifdef Q_OS_MACOS
    m_menuBar = new QMenuBar();
#endif
}

void AppStartup::setupFonts()
{
    auto guiApp = qobject_cast<QGuiApplication*>(m_app);
    if (guiApp) {
        // Load color emoji font:
        QFontDatabase::addApplicationFont(":/Fonts/NotoColorEmoji-unhinted/NotoColorEmoji.ttf");

        // Load Roboto font:
        QFontDatabase::addApplicationFont(":/Fonts/Roboto/Roboto-Bold.ttf");
        QFontDatabase::addApplicationFont(":/Fonts/Roboto/Roboto-Italic.ttf");
        QFontDatabase::addApplicationFont(":/Fonts/Roboto/Roboto-Regular.ttf");
    }

#ifdef OPENTODOLIST_FLATPAK
    {
        QDir dir("/var/config/fontconfig/conf.d");
        if (dir.mkpath(".")) {
            QFile::copy("/app/etc/fonts/conf.d/90-otl-color-emoji.conf", // NOLINT
                        dir.absoluteFilePath("90-otl-color-emoji.conf"));
        }
    }
#endif
}

void AppStartup::parseCommandLineArgs()
{
    qDebug() << "Processing command line" << m_app->arguments();
    m_parser.setApplicationDescription(tr("Manage your personal data."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();

    // Enable touch screen optimizations
    QCommandLineOption enableTouchOption = { { "T", "enable-touch" },
                                             tr("Switch on some optimizations for touchscreens.") };
    m_parser.addOption(enableTouchOption);

    m_parser.addOption({ "service", tr("Only run the app background service") });
    m_parser.addOption({ "gui",
                         tr("Only run the app GUI and connect to an "
                            "existing app background service") });

#ifdef Q_OS_WIN
    m_parser.addOption(
            { "enable-console", tr("Enable a console on Windows to gather debug output") });
#endif

    m_parser.process(*m_app);
    qDebug() << "Finished parsing command line";
}

void AppStartup::exitIfIsSecondaryInstance()
{
#ifdef OTL_USE_SINGLE_APPLICATION
    auto app = static_cast<SingleApplication*>(m_app);
    if (app->isSecondary()) {
        // If this is a GUI only app, allow secondary:
        if (m_parser.isSet("gui") && !m_parser.isSet("service")) {
            return;
        } else {
            ::exit(0);
        }
    }
#endif
}

void AppStartup::openConsole()
{
#ifdef Q_OS_WIN
    if (m_parser.isSet("enable-console")) {
        FreeConsole();
        if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
            AllocConsole();
        }
        (void)freopen("CONOUT$", "w", stdout);
        (void)freopen("CONOUT$", "w", stderr);
        s_prevMessageHandler << qInstallMessageHandler(&AppStartup::debugMessageHandler);
    }
#endif
}

void AppStartup::createCache()
{
    m_cache = new Cache();
    m_cache->initialize();
}

void AppStartup::startBackgroundService()
{
    if (m_parser.isSet("gui") && !m_parser.isSet("service")) {
        // User asked to only start GUI, skip running the background service.
        return;
    }
#if TARGET_OS_SIMULATOR
    // Local socket seems not to work on iOS simulator
    auto tcpServer = new QTcpServer();
    tcpServer->listen(QHostAddress(QStringLiteral("127.0.0.1")), 14782);
    m_srcNode = new QRemoteObjectHost();
    tcpServer->setParent(m_srcNode);
    connect(tcpServer, &QTcpServer::newConnection, m_srcNode, [=]() {
        qDebug() << "New connection from client to QtRO TCP server";
        m_srcNode->addHostSideConnection(tcpServer->nextPendingConnection());
        m_srcNode->enableRemoting(m_backgroundService);
    });
#else
#    ifdef Q_OS_ANDROID
    m_srcNode = new QRemoteObjectHost(QUrl(QStringLiteral("localabstract:opentodolist")));
#    else
    m_srcNode = new QRemoteObjectHost(QUrl(QStringLiteral("local:opentodolist")));
#    endif
#endif
    m_backgroundService = new BackgroundService(m_cache);
    m_srcNode->enableRemoting(m_backgroundService);
}

void AppStartup::startGUI()
{
    if (m_parser.isSet("service") && !m_parser.isSet("gui")) {
        // User asked to only start service, skip running the GUI:
        return;
    }
    m_engine = new QQmlApplicationEngine;
    m_translations = new OpenTodoList::Translations(m_engine);
    QString qmlBase = "qrc:/";
    m_engine->addImportPath(qmlBase);

    m_application = new Application(m_cache);
#ifdef Q_OS_ANDROID
    m_application->setPropagateCacheEventsFromBackgroundService(true);
#endif
    m_qmlPlugin.setApplication(m_application);
    m_qmlPlugin.registerTypes("OpenTodoList");

    m_engine->rootContext()->setContextProperty("application", m_app);
    m_engine->rootContext()->setContextProperty("applicationVersion",
                                                QVariant(OPENTODOLIST_VERSION));
    m_engine->rootContext()->setContextProperty("gitRevision",
#ifdef OPENTODOLIST_GIT_REVISION
                                                QVariant(OPENTODOLIST_GIT_REVISION)
#else
                                                QObject::tr("unknown")
#endif
    );
    m_engine->rootContext()->setContextProperty("gitVersion",
#ifdef OPENTODOLIST_GIT_VERSION
                                                QVariant(OPENTODOLIST_GIT_VERSION)
#else
                                                QObject::tr("unknown")
#endif
    );
    m_engine->rootContext()->setContextProperty("qmlBaseDirectory", qmlBase);
#ifdef OPENTODOLIST_DEBUG
    m_engine->rootContext()->setContextProperty("isDebugBuild", true);
#else
    m_engine->rootContext()->setContextProperty("isDebugBuild", false);
#endif

    auto url = QUrl(qmlBase + "main.qml");

    connect(
            m_engine, &QQmlApplicationEngine::objectCreated, m_app,
            [url](const QObject* obj, const QUrl& objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            },
            Qt::QueuedConnection);

    m_engine->load(url);

    // Handle application state changes
    auto guiApp = qobject_cast<QGuiApplication*>(m_app);
    if (guiApp) {
        connect(guiApp, &QGuiApplication::applicationStateChanged, this,
                [=](Qt::ApplicationState state) {
                    qCDebug(log) << "Application state changed to" << state;
                    switch (state) {
                    case Qt::ApplicationActive:
                        emit m_application->applicationActivated();
                        break;
                    default:
                        break;
                    }
                });
    }
}

void AppStartup::showTrayIcon()
{
    auto* app = qobject_cast<QApplication*>(m_app);
    if (!app) {
        qWarning() << "Running in service process only - NOT creating tray icon";
        return;
    }

    m_trayMenu = new QMenu();
    auto openAction = m_trayMenu->addAction(tr("Open"));
    if (m_backgroundService) {
        connect(openAction, &QAction::triggered, m_backgroundService,
                &BackgroundService::showAppWindowRequested);
    } else if (m_application) {
        connect(openAction, &QAction::triggered, m_application, &Application::showWindowRequested);
    }
    auto quickNotesAction = m_trayMenu->addAction(tr("Quick Note"));
    if (m_backgroundService) {
        connect(quickNotesAction, &QAction::triggered, m_backgroundService,
                &BackgroundService::showQuickNotesEditorRequested);
    } else if (m_application) {
        connect(quickNotesAction, &QAction::triggered, m_application,
                &Application::showQuickNotesEditorRequested);
    }

#ifdef OPENTODOLIST_WITH_KNOTIFICATIONS
    m_statusNotifierItem = new KStatusNotifierItem(app);
    m_statusNotifierItem->setStatus(KStatusNotifierItem::Active);
    m_statusNotifierItem->setIconByName("net.rpdev.OpenTodoList");
    m_statusNotifierItem->setContextMenu(m_trayMenu);
    connect(m_statusNotifierItem, &KStatusNotifierItem::activateRequested, this,
            [=](bool active, QPoint) {
                if (active) {
                    if (m_backgroundService) {
                        emit m_backgroundService->systemTrayIconClicked();
                    } else {
                        emit m_application->systemTrayIconClicked();
                    }
                } else {
                    if (m_backgroundService) {
                        emit m_backgroundService->hideAppWindowRequested();
                    } else {
                        emit m_application->hideWindowRequested();
                    }
                }
            });
#else
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/icons/hicolor/64x64/apps/net.rpdev.OpenTodoList.png"));
    m_trayIcon->setContextMenu(m_trayMenu);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this,
            [=](QSystemTrayIcon::ActivationReason reason) {
                switch (reason) {
                case QSystemTrayIcon::Trigger:
                    if (m_backgroundService) {
                        emit m_backgroundService->systemTrayIconClicked();
                    } else if (m_application) {
                        emit m_application->systemTrayIconClicked();
                    }
                    break;
                default:
                    break;
                }
            });
    m_trayIcon->show();

    if (m_trayIcon->isSystemTrayAvailable()) {
        app->setQuitOnLastWindowClosed(false);
    }

    auto quitAction = m_trayMenu->addAction(tr("Quit"));
    connect(quitAction, &QAction::triggered, m_app, &QCoreApplication::quit);
#endif
}

void AppStartup::debugMessageHandler(QtMsgType type, const QMessageLogContext& context,
                                     const QString& msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char* category = context.category ? context.category : "";
    const char* file = context.file ? context.file : "";
    const char* function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "%s: Debug: %s (%s:%u, %s)\n", category, localMsg.constData(), file,
                context.line, function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "%s: Info: %s (%s:%u, %s)\n", category, localMsg.constData(), file,
                context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s: Warning: %s (%s:%u, %s)\n", category, localMsg.constData(), file,
                context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s: Critical: %s (%s:%u, %s)\n", category, localMsg.constData(), file,
                context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s: Fatal: %s (%s:%u, %s)\n", category, localMsg.constData(), file,
                context.line, function);
        break;
    }
    for (const auto& handler : s_prevMessageHandler) {
        handler(type, context, msg);
    }
}

int AppStartup::exec(int& argc, char* argv[])
{
    qDebug() << "Applying app-wide configs";
    setupGlobals();
    qDebug() << "Creating Qt app";
    createApp(argc, argv);
    qDebug() << "Parsing command line arguments";
    parseCommandLineArgs();
    qDebug() << "Opening console";
    openConsole();
    qDebug() << "Checking for primary app instance...";
    exitIfIsSecondaryInstance();
    qDebug() << "Setting up fonts";
    setupFonts();
    qDebug() << "Printing diagnostics";
    printDiagnostics();
    qDebug() << "Creating cache";
    createCache();
    qDebug() << "Starting background service";
    startBackgroundService();
    qDebug() << "Starting GUI";
    startGUI();
    qDebug() << "Showing tray icon";
    showTrayIcon();
    qDebug() << "Starting application event loop";
    return m_app->exec();
}
