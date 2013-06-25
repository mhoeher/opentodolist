/*
 *  OpenTodoListCore - Core functionality for the OpenTodoList application
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

#ifndef TODOLISTLIBRARY_H
#define TODOLISTLIBRARY_H

#include "opentodolistcore_global.h"
#include "abstracttodolist.h"
#include "pluginsloader.h"
#include "objectmodel.h"
#include "opentodolistinterfaces.h"

#include <QObject>

class OPENTODOLISTCORESHARED_EXPORT TodoListLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QObject* plugins READ plugins CONSTANT )
    Q_PROPERTY( QObject* todoLists READ todoLists CONSTANT )
    Q_PROPERTY( QObject* todos READ todos CONSTANT )
public:
    
    typedef ObjectModel< AbstractTodoList > TodoLists;
    
    explicit TodoListLibrary(QObject *parent = 0);
    virtual ~TodoListLibrary();
    
    PluginsLoader *plugins() const;
    TodoLists* todoLists() const;

    AbstractTodoList::TodoList* todos() const;
    
    Q_INVOKABLE bool createTodoList( const QString& name, OpenTodoListBackend* type );

signals:
    
public slots:

private:

    PluginsLoader                   *m_plugins;
    TodoLists                       *m_lists;

    QList<QVariantMap>               m_nonLoadableLists;
    AbstractTodoList::TodoList      *m_todos;
    
    OpenTodoListBackend* backendByTypeName( const QString& type );
    
private slots:
    
    void saveSettings();
    void restoreSettings();
    
};

#endif // TODOLISTLIBRARY_H
