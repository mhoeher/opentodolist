/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  Martin Höher <martin@rpdev.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "applicationviewer.h"

#include <QQmlEngine>

ApplicationViewer::ApplicationViewer(QWindow* parent) : 
    QtQuick2ApplicationViewer(parent),
    m_watcher( new QFileSystemWatcher( this ) )
{
    connect( m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(reload()) );
    connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(reload()));
}

void ApplicationViewer::addImportPath(const QString& path)
{
    QtQuick2ApplicationViewer::addImportPath( path );
    m_watcher->addPath( path );
}

void ApplicationViewer::setMainFile(const QString& mainFile)
{
    m_mainFile = mainFile;
    reload();
}

void ApplicationViewer::reload()
{
    engine()->clearComponentCache();
    setMainQmlFile( m_mainFile );
}
