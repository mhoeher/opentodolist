#include "libraryfactory.h"

/**
   @brief Constructor.
   
   Creates a new LibraryFactory with the given @p name, @p id @p descrition, @p createComponent
   and @p editComponent. If given, the object will be a child of the given @p parent.
 */
LibraryFactory::LibraryFactory(
    const QString &name,
    const QString &id,
    const QString &description,
    const QString &createComponent,
    const QString &editComponent,
    QObject *parent) : QObject(parent),
  m_name( name ),
  m_id( id ),
  m_description( description ),
  m_createComponent( createComponent ),
  m_editComponent( editComponent )
{
}
