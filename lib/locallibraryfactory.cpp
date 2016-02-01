#include "locallibraryfactory.h"

#include "library.h"


/**
   @brief The ID used by the LocalLibraryFactory class.
 */
const QString LocalLibraryFactory::ID = "LocalLibrary";

/**
   @brief Implements LibraryFactory::createLibrary().
 */
Library *LocalLibraryFactory::createLibrary(
    const QString &name, const QString &directory, QObject *parent) const
{
  return new Library(name, directory, this, parent);  
}

/**
   @brief Constructor.
   
   Creates a new LocalLibraryFactory object with the given @p parent.
 */
LocalLibraryFactory::LocalLibraryFactory(QObject *parent) : 
  LibraryFactory(tr( "Local Collection" ),
                 ID,
                 tr("A collection of items in a local directory."),
                 "CreateLocalLibraryComponent",
                 "EditLocalLibraryComponent",
                 parent)
{
}
