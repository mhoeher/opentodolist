#include "localxmlbackend.h"

#include "localxmltodolistfactory.h"

#include <QtPlugin>

LocalXmlBackend::LocalXmlBackend(QObject *parent) :
    OpenTodoListBackend(parent),
    m_factory( 0 )
{
    setName( tr( "Local XML Directory" ) );
    setDescription( tr( "Stores data in XML files located in a directory on your local host." ) );
    setType( "LocalXmlDirectory" );
    
    m_factory = new LocalXmlTodoListFactory( type(), this );
}

TodoListFactory *LocalXmlBackend::factory()
{
    return m_factory;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(LocalXmlBackend, LocalXmlBackend)
#endif // QT_VERSION < 0x050000
