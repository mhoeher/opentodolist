#ifndef APPLICATION_H
#define APPLICATION_H

#include "library.h"
#include "libraryfactory.h"

#include <QObject>
#include <QQmlListProperty>
#include <QSettings>
#include <QStringList>
#include <QUrl>
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
  Q_PROPERTY(QQmlListProperty<Library> libraries READ libraryList NOTIFY librariesChanged)
  Q_PROPERTY(Library* defaultLibrary READ defaultLibrary NOTIFY defaultLibraryChanged)
public:
  
  explicit Application(QObject *parent = 0);
  explicit Application(QSettings *settings, QObject *parent = 0);
  
  virtual ~Application();
  
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
  Q_INVOKABLE Library *addLocalLibrary(const QString &name, const QString &directory);
  Q_INVOKABLE Library *addLocalLibrary(const QString &directory);
  Q_INVOKABLE Library *addLocalLibrary(const QUrl &directory);
  QQmlListProperty<Library> libraryList();
  
  Q_INVOKABLE void saveValue(const QString &name, const QVariant &value);
  Q_INVOKABLE QVariant loadValue(const QString &name, const QVariant &defaultValue = QVariant());
  
  Q_INVOKABLE QString readFile(const QString &fileName) const;
  Q_INVOKABLE QVariant find3rdPartyInfos() const;

  Q_INVOKABLE QString urlToLocalFile(const QUrl &url) const;
  Q_INVOKABLE QUrl localFileToUrl(const QString &localFile) const;
  
  Library *defaultLibrary();
  
signals:
  
  void librariesChanged();
  void defaultLibraryChanged();
  
public slots:
  
private:
  
  Libraries               m_libraries;
  Library                *m_defaultLibrary;
  LibraryFactories        m_libraryFactories;
  QSettings              *m_settings;
  bool                    m_loadingLibraries;
  bool                    m_isInCustomLocation;
  
  void createFactories();
  void saveLibraries();
  void loadLibraries();
  
  static Library* librariesAt(QQmlListProperty<Library> *property, int index);
  static int librariesCount(QQmlListProperty<Library> *property);
  
  QString defaultLibraryLocation() const;
  void runMigrations();
  
private slots:
  
  void onLibraryDeleted(Library *library);
  
};

#endif // APPLICATION_H
