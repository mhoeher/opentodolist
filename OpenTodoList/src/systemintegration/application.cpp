/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "application.h"

#include "core/coreplugin.h"
#include "datamodel/datamodelplugin.h"
#include "database/databaseplugin.h"
#include "models/modelsplugin.h"
#include "systemintegration/systemintegrationplugin.h"

#include <QDir>
#include <QProcess>
#include <QQmlEngine>
#include <QQmlContext>

namespace OpenTodoList {
namespace SystemIntegration {

Application::Application(int &argc, char *argv[]) :
  ApplicationBase(argc, argv),
  m_instance( nullptr ),
  m_handler( nullptr ),
  m_viewer( nullptr ),
  m_notifier( nullptr ),
  m_database( nullptr ),
  m_pluginsRegistered( false )
{
}

Application::~Application()
{
  if ( m_viewer ) {
    delete m_viewer;
  }
}

void Application::prepare()
{
  // keep app open in background
  setQuitOnLastWindowClosed( false );

  m_handler = new CommandHandler( this );
  connect( m_handler, &CommandHandler::requestCreateWindow, [this] { showWindow(); } );

  // ensure app is running at most once
  m_instance = new ApplicationInstance( QCoreApplication::applicationName() );
  if ( m_instance->state() == ApplicationInstance::InstanceIsSecondary ) {
    qDebug() << "Running secondary instance. Contacting server and quitting...";
    m_instance->sendMessage( SystemIntegration::CommandHandler::show() );
  } else {
    // process incoming requests in command handler:
    connect( m_instance, &ApplicationInstance::receivedMessage,
             m_handler, &CommandHandler::handleMessage );

    // further setup:
    setupPaths();
    showNotifierIcon();

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACX) && !defined(Q_OS_ANDROID)
    setWindowIcon( QIcon( m_basePath + "/../share/OpenTodoList/icons/OpenTodoList80.png" ) );
#endif
  }

  // start database
  if ( m_instance->state() == ApplicationInstance::InstanceIsPrimary ) {
    m_database = new DataBase::Database( this );
  }
}

ApplicationInstance *Application::instance() const
{
  return m_instance;
}

CommandHandler *Application::handler() const
{
  return m_handler;
}

QtQuick2ApplicationViewer *Application::viewer() const
{
  return m_viewer;
}

StatusNotifierIcon *Application::notifierIcon() const
{
  return m_notifier;
}

/**
   @brief The application wide todo database
 */
DataBase::Database *Application::database() const
{
  return m_database;
}

void Application::registerPlugins()
{
  if ( !m_pluginsRegistered ) {
    // Register "Core" plugin:
    OpenTodoList::Core::Plugin corePlugin;
    corePlugin.registerTypes( "net.rpdev.OpenTodoList.Core" );

    // Register "DataModel" plugin:
    OpenTodoList::DataModel::Plugin dataModelPlugin;
    dataModelPlugin.registerTypes( "net.rpdev.OpenTodoList.DataModel" );

    // Register "DataBase" plugin:
    OpenTodoList::DataBase::Plugin dataBasePlugin;
    dataBasePlugin.registerTypes( "net.rpdev.OpenTodoList.DataBase" );

    // Register "Models" plugin:
    OpenTodoList::Models::Plugin modelsPlugin;
    modelsPlugin.registerTypes( "net.rpdev.OpenTodoList.Models" );

    // Register "SystemIntegration" plugin:
    OpenTodoList::SystemIntegration::Plugin systemIntegrationPlugin;
    systemIntegrationPlugin.registerTypes( "net.rpdev.OpenTodoList.SystemIntegration" );

    m_pluginsRegistered = true;
  }
}

void Application::showWindow()
{
  QMetaObject::invokeMethod( this, "showWindowImplementation", Qt::QueuedConnection );
}

void Application::hideWindow()
{
  QMetaObject::invokeMethod( this, "hideWindowImplementation", Qt::QueuedConnection );
}

void Application::setupPaths( QtQuick2ApplicationViewer *viewer )
{
  // Add plugin search paths for QML plugins
#ifdef Q_OS_ANDROID
  QCoreApplication::addLibraryPath( QCoreApplication::applicationDirPath() );
  if ( viewer ) {
    viewer->engine()->addPluginPath( QCoreApplication::applicationDirPath() );
  }
  m_basePath = applicationDirPath();
#else
  QString m_basePath = applicationDirPath();
#ifdef Q_OS_MACX
  QString pluginsDirName = "PlugIns";
#else
  QString pluginsDirName = "plugins";
#endif
  QDir baseDir( m_basePath );
  if ( baseDir.cdUp() ) {
    if ( baseDir.cd( pluginsDirName ) ) {
      if ( !viewer ) {
        QCoreApplication::addLibraryPath( baseDir.absolutePath() );
      } else {
        foreach ( QString entry, baseDir.entryList( QDir::Dirs ) ) {
          viewer->engine()->addPluginPath( baseDir.absoluteFilePath( entry ) );
        }
      }
    }
  }
#endif
}

void Application::showNotifierIcon()
{
  m_notifier = new StatusNotifierIcon( m_handler );
  m_notifier->setApplicationTitle( applicationName() );
#if !defined(Q_OS_MACX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
  m_notifier->setApplicationIcon( QCoreApplication::applicationDirPath() +
                                  "/../share/OpenTodoList/icons/OpenTodoList80.png" );
#else
  m_notifier->setApplicationIcon( "qrc:/qml/OpenTodoList/OpenTodoList.png" );
#endif
  m_notifier->show();
}

void Application::showWindowImplementation()
{
  if ( !m_viewer ) {
    m_viewer = new QtQuick2ApplicationViewer();
    m_handler->setApplicationWindow( m_viewer );
    setupPaths(m_viewer);
    registerPlugins();
    m_viewer->engine()->rootContext()->setContextProperty( "application", QVariant::fromValue< QObject* >( this ) );
    m_viewer->addImportPath( QStringLiteral("qrc:/qml") );
    m_viewer->setMainQmlFile("qrc:/qml/OpenTodoList/main.qml");
    m_viewer->showExpanded();
    emit viewerChanged();
  }
}

void Application::hideWindowImplementation()
{
  if ( m_viewer ) {
    m_viewer->deleteLater();
    m_viewer = nullptr;
    emit viewerChanged();
  }
}

} // SystemIntegration
} // OpenTodoList
