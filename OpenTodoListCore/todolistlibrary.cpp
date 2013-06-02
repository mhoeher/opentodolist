#include "todolistlibrary.h"

#include "todolistfactory.h"

#include <QCoreApplication>
#include <QStringList>
#include <QSettings>

TodoListLibrary::TodoListLibrary(QObject *parent) :
    QObject(parent),
    m_plugins( new PluginsLoader( this ) ),
    m_lists( new TodoLists( this ) ),
    m_nonLoadableLists()
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

bool TodoListLibrary::createTodoList(const QString& name, OpenTodoListBackend* type)
{
    if ( !name.isEmpty() && type ) {
        AbstractTodoList* list = type->factory()->createTodoList( this );
        list->setName( name );
        m_lists->append( list );
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
