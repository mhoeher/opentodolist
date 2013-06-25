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

#include "todolistlibrary.h"

#include "abstracttodo.h"
#include "abstracttodolist.h"
#include "todolistfactory.h"

#include <QCoreApplication>
#include <QStringList>
#include <QSettings>

TodoListLibrary::TodoListLibrary(QObject *parent) :
    QObject(parent),
    m_plugins( new PluginsLoader( this ) ),
    m_lists( new TodoLists( this ) ),
    m_nonLoadableLists(),
    m_todos( new AbstractTodoList::TodoList( this ) )
{
    restoreSettings();
    connect( QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(saveSettings()) );
}

TodoListLibrary::~TodoListLibrary()
{
}

PluginsLoader *TodoListLibrary::plugins() const
{
    return m_plugins;
}

TodoListLibrary::TodoLists* TodoListLibrary::todoLists() const
{
    return m_lists;
}

AbstractTodoList::TodoList *TodoListLibrary::todos() const
{
    return m_todos;
}

bool TodoListLibrary::createTodoList(const QString& name, OpenTodoListBackend* type)
{
    if ( !name.isEmpty() && type ) {
        AbstractTodoList* list = type->factory()->createTodoList( this );
        list->setName( name );
        m_lists->append( list );
        m_todos->append( list->todos() );
        return true;
    }
    return false;
}

OpenTodoListBackend* TodoListLibrary::backendByTypeName(const QString& type)
{
    foreach ( OpenTodoListBackend* backend, m_plugins->backends()->data() ) {
        if ( backend->type() == type ) {
            return backend;
        }
    }
    return 0;
}

void TodoListLibrary::saveSettings()
{
    QSettings settings;
    settings.beginWriteArray( "todoLists", m_lists->data().size() + m_nonLoadableLists.size() );
    for ( int i = 0; i < m_lists->data().size(); ++i ) {
        settings.setArrayIndex( i );
        AbstractTodoList* list = m_lists->data().at( i );
        settings.setValue( "type", list->type() );
        settings.setValue( "key", list->key() );
    }
    for ( int i = 0; i < m_nonLoadableLists.size(); ++i ) {
        settings.setArrayIndex( m_lists->data().size() + i );
        QVariantMap backup = m_nonLoadableLists.at( i );
        foreach ( QString key, backup.keys() ) {
            settings.setValue( key, backup.value( key ) );
        }
    }
    settings.endArray();
    settings.sync();
}

void TodoListLibrary::restoreSettings()
{
    QSettings settings;
    int numLists = settings.beginReadArray( "todoLists" );
    for ( int i = 0; i < numLists; ++i ) {
        settings.setArrayIndex( i );
        OpenTodoListBackend* backend = backendByTypeName( settings.value( "type" ).toString() );
        if ( backend ) {
            AbstractTodoList* list = backend->factory()->createTodoList( this, settings.value( "key" ).toString() );
            if ( list ) {
                m_lists->append( list );
            }
        } else {
            QVariantMap backupData;
            foreach ( QString key, settings.allKeys() ) {
                backupData.insert( key, settings.value( key ) );
            }
            m_nonLoadableLists << backupData;
        }
    }
    settings.endArray();
}
