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
#include <QJsonDocument>
#include <QQmlEngine>
#include <QSettings>

ApplicationViewer::ApplicationViewer(const QString &basePath, QObject *parent) :
    QtQuick2ApplicationViewer(parent),
    m_watcher( new QFileSystemWatcher( this ) ),
    m_basePath( basePath ),
    m_styleInfo( loadStyleInfo( basePath ) ),
    m_currentStyle(),
    m_showing( false )
{
    addImageProvider( "primitives", new ImageProvider() );
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

QString ApplicationViewer::currentStyle() const
{
    return m_currentStyle;
}

void ApplicationViewer::setCurrentStyle(const QString &currentStyle)
{
    if ( m_currentStyle != currentStyle || m_currentStyle.isEmpty() ) {
        m_currentStyle = currentStyle;
        emit currentStyleChanged();
    }
}

void ApplicationViewer::show()
{
    if ( !m_showing ) {
        m_showing = true;
        connect( m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(reload()) );
        connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(reload()));
        connect( this, SIGNAL(currentStyleChanged()), this, SLOT(reload()) );
    }
    reload();
}

QString ApplicationViewer::platformDefaultStyle() const
{
#ifdef Q_OS_ANDROID
    return "neutral";
#else
    return "widget";
#endif
}

QVariantList ApplicationViewer::styleInfo() const
{
    return m_styleInfo;
}

void ApplicationViewer::reload()
{
    emit beforeReload();
    clearComponentCache();
    QString style = currentStyle();
    if ( style.isEmpty() ) {
        style = platformDefaultStyle();
    }
    QString mainFileDir = QDir( m_basePath ).absoluteFilePath( style );
    QString mainFileUrl( QDir( mainFileDir ).absoluteFilePath( "main.qml" ) );
    addImportPath( m_basePath );
    if ( mainFileDir.startsWith( ":" ) ) {
        load( QUrl( "qrc" + mainFileUrl ) );
    } else {
        load( mainFileUrl );
    }
    emit reloaded();
}

void ApplicationViewer::loadSettings()
{
    QSettings settings;
    settings.beginGroup( "ApplicationViewer" );
    settings.beginGroup( "Settings" );
    setCurrentStyle( settings.value( "currentStyle", m_currentStyle ).toString());
    settings.endGroup();
    settings.endGroup();
}

void ApplicationViewer::saveSettings()
{
    QSettings settings;
    settings.beginGroup( "ApplicationViewer" );
    settings.beginGroup( "Settings" );
    settings.setValue( "currentStyle", m_currentStyle );
    settings.endGroup();
    settings.endGroup();
}

QVariantList ApplicationViewer::loadStyleInfo(const QString &stylesDir)
{
    QVariantList result;
    QDir dir( stylesDir );
    foreach ( QString entry, dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot ) ) {
        QFile file( dir.absoluteFilePath( entry + "/info.json" ) );
        if ( file.exists() && file.open( QIODevice::ReadOnly ) ) {
          QJsonParseError error;
          QJsonDocument doc = QJsonDocument::fromJson( file.readAll(), &error );
          if ( doc.isNull() ) {
              qWarning() << "Failed to load style info:" << error.errorString();
          } else {
              QVariantMap data = doc.toVariant().toMap();
              data.insert( "id", entry );
              result.append( data );
          }
          file.close();
        }
    }
    return result;
}
