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

#ifndef OPENTODOLIST_APPSTARTUP_H_
#define OPENTODOLIST_APPSTARTUP_H_

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
#include <QMenu>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QRemoteObjectHost>
#include <QScopedPointer>
#include <QScreen>
#include <QSslSocket>
#include <QSysInfo>
#include <QSystemTrayIcon>

#ifdef Q_OS_ANDROID
#    include <QtAndroidExtras>
#endif

#include <iostream>

#ifdef Q_OS_WIN
#    include <windows.h>
#endif

#include "application.h"
#include "datastorage/cache.h"
#include "opentodolistqmlextensionsplugin.h"
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
    Q_OBJECT

public:
    AppStartup();
    ~AppStartup() override;

    int exec(int& argc, char* argv[]);

private:
    QCoreApplication* m_app;
    Cache* m_cache;
    QRemoteObjectHost* m_srcNode;
    BackgroundService* m_backgroundService;
    Application* m_application;
    QSystemTrayIcon* m_trayIcon;
    QMenu* m_trayMenu;
    static QVector<QtMessageHandler> s_prevMessageHandler;

    QCommandLineParser m_parser;
    QQmlApplicationEngine* m_engine;
    OpenTodoList::Translations* m_translations;
    OpenTodoListQmlExtensionsPlugin m_qmlPlugin;

    void setupGlobals();
    void createApp(int& argc, char* argv[]);
    void setupFonts();
    void parseCommandLineArgs();
    void exitIfIsSecondaryInstance();
    void openConsole();
    void printDiagnostics();
    void createCache();
    void startBackgroundService();
    void startGUI();
    void showTrayIcon();
    static void debugMessageHandler(QtMsgType type, const QMessageLogContext& context,
                                    const QString& msg);
};

#endif // OPENTODOLIST_APPSTARTUP_H_
