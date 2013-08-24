/*
 *  OpenTodoListDesktopQml - Desktop QML frontend for OpenTodoList
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

#include <QDir>
#include <QQmlEngine>

ApplicationViewer::ApplicationViewer(QWindow* parent) : 
    QtQuick2ApplicationViewer(parent),
    m_watcher( new QFileSystemWatcher( this ) )
{
    engine()->addImageProvider( "primitives", new ImageProvider() );
    connect( m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(reload()) );
    connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(reload()));
}

#include <QDebug>

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

void ApplicationViewer::setMainFile(const QString& mainFile)
{
    m_mainFile = mainFile;
    reload();
}

void ApplicationViewer::reload()
{
    engine()->clearComponentCache();
    releaseResources();
    setMainQmlFile( m_mainFile );
}
