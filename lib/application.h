#ifndef APPLICATION_H
#define APPLICATION_H

#include "library.h"
#include "libraryfactory.h"

#include <QObject>
#include <QStringList>
#include <QVector>


/**
   @brief The main class of the application
   
   The Application class is used as entry point into the OpenTodoList application. It is used
   as contained class and provides references to other objects. Basically, the Application class
   models the application, i.e. it is created when the application starts and destroyed once
   the application is to be closed.
 */
class Application : public QObject
{
  Q_OBJECT
  Q_PROPERTY(LibraryFactories libraryFactories READ libraryFactories CONSTANT)
  Q_PROPERTY(Libraries libraries READ libraries CONSTANT)
public:
  explicit Application(QObject *parent = 0);
  
  /**
     @brief The list of all registered LibraryFactory objects.
   */
  LibraryFactories libraryFactories() const { return m_libraryFactories; }
  
  /**
     @brief The list of all Library objects present in the application.
   */
  Libraries libraries() const               { return m_libraries; }

  QStringList libraryTypes() const;
  LibraryFactory* libraryFactoryForType(const QString &factoryId) const;
  
  
signals:
  
public slots:
  
private:
  
  Libraries               m_libraries;
  LibraryFactories        m_libraryFactories;
  
};

#endif // APPLICATION_H
