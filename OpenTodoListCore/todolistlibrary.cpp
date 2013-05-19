#include "todolistlibrary.h"

TodoListLibrary::TodoListLibrary(QObject *parent) :
    QObject(parent),
    m_plugins( new PluginsLoader( this ) )
{
}

PluginsLoader *TodoListLibrary::plugins() const
{
    return m_plugins;
}

