/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013  Martin Höher <martin@rpdev.net>
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

#include "applicationviewer.h"

#include "imageprovider.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QQmlEngine>
#include <QSettings>

ApplicationViewer::ApplicationViewer(const QString &basePath, QWindow *parent) :
    QtQuick2ApplicationViewer(parent),
    m_watcher( new QFileSystemWatcher( this ) ),
    m_basePath( basePath )
{
    engine()->addImageProvider( "primitives", new ImageProvider() );
}

ApplicationViewer::~ApplicationViewer()
{
}

void ApplicationViewer::addImportPath(const QString& path)
{
    QtQuick2ApplicationViewer::addImportPath( path );
    if ( !( path.startsWith( "qrc:" ) || path.startsWith( ":" ) ) ) {
        QStringList pathsToWatch;
        pathsToWatch << path;
        while ( !pathsToWatch.isEmpty() ) {
            m_watcher->addPath( pathsToWatch.first() );
            QDir dir( pathsToWatch.first() );
            foreach ( QString entry, dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks ) ) {
                pathsToWatch << dir.absoluteFilePath( entry );
            }
            pathsToWatch.removeFirst();
        }
    }
}

QString ApplicationViewer::basePath() const
{
    return m_basePath;
}

// inspired by http://vizzzion.org/blog/2014/02/reasonable-dpi-in-plasma-next/
// to let application know currently set font metrics
int ApplicationViewer::mWidth() const
{
    return QFontMetrics( QGuiApplication::font() ).boundingRect( "M" ).width();
}

// inspired by http://vizzzion.org/blog/2014/02/reasonable-dpi-in-plasma-next/
// to let application know currently set font metrics
int ApplicationViewer::mHeight() const
{
    return QFontMetrics( QGuiApplication::font() ).boundingRect( "M" ).height();
}

/**
   @brief Returns whether the application is build in debug mode

   This returns true in case the application has been built
   in debug mode or false otherwise.
 */
bool ApplicationViewer::isDebug() const
{
#ifdef QT_DEBUG
    return true;
#else
    return false;
#endif
}

void ApplicationViewer::show()
{
    if ( !m_showing ) {
        m_showing = true;
        connect( m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(reload()) );
        connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(reload()));
    }
    reload();
    QtQuick2ApplicationViewer::show();
}

void ApplicationViewer::reload()
{
    emit beforeReload();
    engine()->clearComponentCache();
    QDir mainFileDir( m_basePath );
    QString mainFileUrl( mainFileDir.absoluteFilePath( "main.qml" ) );
    addImportPath( m_basePath );
    if ( m_basePath.startsWith( ":" ) ) {
        setMainQmlFile( "qrc" + mainFileUrl );
    } else {
        setMainQmlFile( mainFileUrl );
    }
    emit reloaded();
}

void ApplicationViewer::loadSettings()
{
    QSettings settings;
    settings.beginGroup( "ApplicationViewer" );
    setWidth( settings.value( "width", width() ).toInt() );
    setHeight( settings.value( "height", height() ).toInt() );
    setPosition( settings.value( "x", x() ).toInt(),
                 settings.value( "y", y() ).toInt() );
    settings.endGroup();
}

void ApplicationViewer::saveSettings()
{
    QSettings settings;
    settings.beginGroup( "ApplicationViewer" );
    settings.setValue( "width", width() );
    settings.setValue( "height", height() );
    settings.setValue( "x", x() );
    settings.setValue( "y", y() );
    settings.endGroup();
}
