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

#ifndef PLUGINSLOADER_H
#define PLUGINSLOADER_H

#include "opentodolistinterfaces.h"
#include "objectmodel.h"

#include <QList>
#include <QObject>
#include <QObjectList>
#include <QSet>

class PluginsLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QObject* backends READ backends CONSTANT )
public:
    
    typedef QSet< QString > NameSet;
    typedef ObjectModel<OpenTodoListBackend> Backends;

    typedef QVector<BackendInterface*> BackendInterfaces;
    
    explicit PluginsLoader(QObject *parent = 0);
    
    Backends* backends() const;

    BackendInterfaces backendInterfaces() const;

signals:
    
public slots:

private:

    Backends* m_backends;
    NameSet   m_loadedBackends;

    BackendInterfaces m_backendInterfaces;


    void addBackend( QObject* o );
    void addBackendInterface( QObject* plugin );
    
};

#endif // PLUGINSLOADER_H
