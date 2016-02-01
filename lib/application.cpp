#include "application.h"

#include "locallibraryfactory.h"

/**
   @brief Constructor.
   
   Creates a new Application object. The instance will be a child of the given @p parent.
 */
Application::Application(QObject *parent) : QObject(parent)
{
  m_libraryFactories << new LocalLibraryFactory(this);
}

/**
   @brief Return the list of all library factory IDs.
   
   This method returns the list of all LibraryFactory IDs (cf LibraryFactory::id()) registered
   in the Application.
   
   @sa Application::libraryFactoryForType()
 */
QStringList Application::libraryTypes() const
{
  QStringList result;
  for (auto factory : m_libraryFactories) {
    result << factory->id();
  }
  result.sort();
  return result;
}

/**
   @brief Return the LibraryFactory for a given type of library.
   
   This method returns the LibraryFactory where the LibraryFactory::id() equals the
   given @p factoryId. If no such library is found, returns a nullptr.
   @param factoryId
   @return 
 */
LibraryFactory *Application::libraryFactoryForType(const QString &factoryId) const
{
  for (auto factory : m_libraryFactories) {
    if (factory->id() == factoryId) {
      return factory;
    }
  }
  return nullptr;
}


