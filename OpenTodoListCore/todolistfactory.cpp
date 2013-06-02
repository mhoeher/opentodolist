#include "todolistfactory.h"

TodoListFactory::TodoListFactory(const QString& type, QObject* parent) :
    QObject(parent),
    m_type( type )
{
}
