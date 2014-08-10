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

#include <QList>
#include <QObject>
#include <QObjectList>
#include <QVector>

class PluginsLoader : public QObject
{
    Q_OBJECT
public:
    
    typedef QVector<BackendInterface*> BackendInterfaces;
    
    explicit PluginsLoader(QObject *parent = 0);
    
    BackendInterfaces backendInterfaces() const;

signals:
    
public slots:

private:

    BackendInterfaces m_backendInterfaces;

    void addBackendInterface( QObject* plugin );
    
};

#endif // PLUGINSLOADER_H
