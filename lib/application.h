#ifndef APPLICATION_H
#define APPLICATION_H

#include "library.h"
#include "libraryfactory.h"

#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QVariantMap>
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
  explicit Application(QSettings *settings, QObject *parent = 0);
  
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
  Q_INVOKABLE Library *addLibrary(const QString &factoryId,
                                  const QString &name,
                                  const QString &directory,
                                  const QVariantMap &args = QVariantMap());
  
  
signals:
  
public slots:
  
private:
  
  Libraries               m_libraries;
  LibraryFactories        m_libraryFactories;
  QSettings              *m_settings;
  bool                    m_loadingLibraries;
  
  void createFactories();
  void saveLibraries();
  void loadLibraries();
  
};

#endif // APPLICATION_H
