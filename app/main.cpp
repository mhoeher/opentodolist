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
#include <QScreen>
#include <QSslSocket>
#include <QSysInfo>

#include <iostream>

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

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // qSetMessagePattern("%{file}(%{line}): %{message}");
#ifdef OPENTODOLIST_DEBUG
    QLoggingCategory(nullptr).setEnabled(QtDebugMsg, true);
#endif

#ifdef OTL_USE_SINGLE_APPLICATION
    SingleApplication app(argc, argv, false,
                          SingleApplication::User | SingleApplication::ExcludeAppPath);
#else
    QGuiApplication app(argc, argv);
#endif

    // Load color emoji font:
    QFontDatabase::addApplicationFont(":/Fonts/NotoColorEmoji-unhinted/NotoColorEmoji.ttf");

// Use Noto Color Emoji as substitution font for color emojies:
// {
//     const auto FontTypes = {
//         QFontDatabase::GeneralFont,
//         QFontDatabase::FixedFont,
//         QFontDatabase::TitleFont,
//         QFontDatabase::SmallestReadableFont
//     };
//     for (auto fontType : FontTypes) {
//         auto font = QFontDatabase::systemFont(fontType);
//         QFont::insertSubstitution(font.family(), "Noto Color Emoji");
//     }
// }
#ifdef OPENTODOLIST_FLATPAK
    {
        QDir dir("/var/config/fontconfig/conf.d");
        if (dir.mkpath(".")) {
            QFile::copy("/app/etc/fonts/conf.d/90-otl-color-emoji.conf",
                        dir.absoluteFilePath("90-otl-color-emoji.conf"));
        }
    }
#endif

    QCoreApplication::setApplicationName("OpenTodoList");
    QCoreApplication::setApplicationVersion(OPENTODOLIST_VERSION);
    QCoreApplication::setOrganizationDomain("www.rpdev.net");
    QCoreApplication::setOrganizationName("RPdev");

    app.setWindowIcon(QIcon(":/icons/hicolor/128x128/apps/net.rpdev.OpenTodoList.png"));

    QCommandLineParser parser;
    parser.setApplicationDescription(
            QCoreApplication::translate("main", "Manage your personal data."));
    parser.addHelpOption();
    parser.addVersionOption();

#ifdef OPENTODOLIST_DEBUG
    QCommandLineOption qmlRootOption = { { "Q", "qml-root" },
                                         QCoreApplication::translate("main", "QML Root Directory"),
                                         QCoreApplication::translate("main", "DIR") };
    parser.addOption(qmlRootOption);
#endif

    // Enable touch screen optimizations
    QCommandLineOption enableTouchOption = {
        { "T", "enable-touch" },
        QCoreApplication::translate("main", "Switch on some optimizations for touchscreens.")
    };
    parser.addOption(enableTouchOption);

    // If APPDIR is set, assume we are running from AppImage.
    // Provide option to remove desktop integration. Note that we
    // don't handle this; instead it is handled by the `desktopintegration`
    // script we use.
    // See https://github.com/AppImage/AppImageKit/blob/master/desktopintegration.
#ifdef OPENTODOLIST_IS_APPIMAGE
    QCommandLineOption removeDesktopIntegrationOption = {
        "remove-appimage-desktop-integration",
        QCoreApplication::translate("main", "Remove shortcuts to the AppImage.")
    };
    parser.addOption(removeDesktopIntegrationOption);

#endif

    parser.process(app);

    QQmlApplicationEngine engine;
    OpenTodoList::Translations translations(&engine);
    QString qmlBase = "qrc:/";

#ifdef OPENTODOLIST_DEBUG
    if (parser.isSet(qmlRootOption)) {
        qmlBase = QDir(parser.value(qmlRootOption)).canonicalPath() + "/";
        if (!QFile::exists(qmlBase + "main.qml")) {
            qFatal("File main.qml does not exist in %s, probably not a valid OpenTodoList QML dir!",
                   qUtf8Printable(qmlBase));
        }
    }
#endif

    engine.addImportPath(qmlBase);
    OpenTodoListQmlExtensionsPlugin plugin;
    Cache cache;
    QRemoteObjectHost srcNode(QUrl(QStringLiteral("local:opentodolist")));
    BackgroundService backgroundService(&cache);
    srcNode.enableRemoting(&backgroundService);
    Application application(&cache);
    plugin.setApplication(&application);
    plugin.registerTypes("OpenTodoList");

#ifdef OTL_USE_SINGLE_APPLICATION
    engine.rootContext()->setContextProperty("application", &app);
#endif
    engine.rootContext()->setContextProperty("applicationVersion", QVariant(OPENTODOLIST_VERSION));
    engine.rootContext()->setContextProperty("qmlBaseDirectory", qmlBase);
#ifdef OPENTODOLIST_DEBUG
    engine.rootContext()->setContextProperty("isDebugBuild", true);
#else
    engine.rootContext()->setContextProperty("isDebugBuild", false);
#endif

    auto url = QUrl(qmlBase + "main.qml");

    QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreated, &app,
            [url](QObject* obj, const QUrl& objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            },
            Qt::QueuedConnection);

    engine.load(url);

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

    return app.exec();
}
