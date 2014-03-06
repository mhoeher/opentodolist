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

#ifndef APPLICATIONVIEWER_H
#define APPLICATIONVIEWER_H

#include "qtquick2applicationviewer/qtquick2applicationviewer.h"

#include <QFileSystemWatcher>
#include <QVariantMap>

class ApplicationViewer : public QtQuick2ApplicationViewer
{
    Q_OBJECT
    Q_PROPERTY( QString basePath READ basePath CONSTANT )
    Q_PROPERTY( int mWidth READ mWidth CONSTANT )
    Q_PROPERTY( int mHeight READ mHeight CONSTANT )
    Q_PROPERTY( bool isDebug READ isDebug CONSTANT )
    
public:
    
    explicit ApplicationViewer(const QString& basePath, QObject *parent = 0);
    virtual ~ApplicationViewer();
    
    void addImportPath( const QString& path );

    QString basePath() const;

    int mWidth() const;
    int mHeight() const;
    bool isDebug() const;

signals:

    void beforeReload();
    void reloaded();

public slots:

    void show();
    void reload();
    void loadSettings();
    void saveSettings();
    
private:

    QFileSystemWatcher* m_watcher;    
    QString m_basePath;
    bool m_showing;

private slots:
    
};

#endif // APPLICATIONVIEWER_H
