#include "localxmlbackend.h"

#include <QtPlugin>

LocalXmlBackend::LocalXmlBackend(QObject *parent) :
    QObject(parent),
    m_factory( new TodoListFactory( this ) )
{
}

TodoListFactory *LocalXmlBackend::factory()
{
    return m_factory;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(LocalXmlBackend, LocalXmlBackend)
#endif // QT_VERSION < 0x050000
