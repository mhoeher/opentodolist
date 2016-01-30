#include "library.h"

/**
   @brief Set the name of the library.
 */
void Library::setName(const QString &name)
{
  if ( m_name != name ) {
    m_name = name;
    emit nameChanged();
  }
}

/**
   @brief Set the directory for which the library returns objects.
 */
void Library::setDirectory(const QString &directory)
{
  if ( m_directory != directory ) {
    // TODO: Trigger search for new/changed items
    m_directory = directory;
    emit directoryChanged();
  }
}

/**
   @brief Constructor.
   
   Creates a new library object with the given @p name and the @p directory. The library
   is created by the @p factory. The object is a child of the given @p parent.
 */
Library::Library(const QString &name, 
                 const QString &directory,
                 const LibraryFactory *factory,
                 QObject *parent) : QObject(parent),
  m_name( name ),
  m_directory( directory ),
  m_factory( factory )
{
}
