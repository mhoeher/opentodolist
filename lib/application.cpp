#include "application.h"

#include "locallibraryfactory.h"

#include "migrator_1_x_to_2_x.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

/**
   @brief Constructor.
   
   Creates a new Application object. The instance will be a child of the given @p parent.
 */
Application::Application(QObject *parent) : QObject(parent),
  m_defaultLibrary(nullptr),
  m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                           QCoreApplication::organizationName(),
                           QCoreApplication::applicationName(), this)),
  m_loadingLibraries(false),
  m_isInCustomLocation(false)
{
  Q_CHECK_PTR(m_settings);
  createFactories();
  loadLibraries();
}

Application::Application(QSettings *settings, QObject *parent) :
  QObject(parent),
  m_settings(settings),
  m_loadingLibraries(false),
  m_isInCustomLocation(true)
{
  Q_CHECK_PTR(m_settings);
  createFactories();
  loadLibraries();
}

Application::~Application()
{
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

/**
   @brief Create a new library.
   
   This method creates a new library using the factory identified by the @p factoryId
   and passing it the @p args.
 */
Library *Application::addLibrary(const QString &factoryId,
                                 const QString &name,
                                 const QString &directory,
                                 const QVariantMap &args)
{
  auto factory = libraryFactoryForType(factoryId);
  if (factory) {
    auto library = factory->createLibrary(name, directory, args, this);
    if (library) {
      m_libraries.append(library);
      connect(library, &Library::libraryDeleted, this, &Application::onLibraryDeleted);
      saveLibraries();
      emit librariesChanged();
      return library;
    }
  }
  return nullptr;
}

/**
   @brief Add a local library.
   
   This is a convenience wrapper which calls addLibrary() with the ID LocalLibraryFactory::ID.
   Hence, this basically adding a local directory of a library to the application.
 */
Library *Application::addLocalLibrary(const QString &name, const QString &directory)
{
  return addLibrary(LocalLibraryFactory::ID, name, directory);
}

/**
   @brief Add a local library.
   
   This is an overloaded version of the addLocalLibrary() method. This version uses the directory
   name of the @p directory also as name of the library.
 */
Library *Application::addLocalLibrary(const QString &directory)
{
  QFileInfo fi(directory);
  return addLocalLibrary(fi.fileName(), directory);
}

/**
   @brief Add a local library.
   
   This is an overloaded version of addLocalLibrary(), which takes the @p directory as a URL.
 */
Library *Application::addLocalLibrary(const QUrl &directory)
{
  return addLocalLibrary(directory.toLocalFile());
}

/**
   @brief Returns the libraries as a QML list property.
 */
QQmlListProperty<Library> Application::libraryList()
{
  return QQmlListProperty<Library>(this, nullptr, librariesCount, librariesAt);
}

/**
   @brief Save a value to the application settings
   
   This method is used to save a value to the application settings. Settings can be restored
   (e.g. when the app restarts).
 */
void Application::saveValue(const QString &name, const QVariant &value)
{
  m_settings->beginGroup("ApplicationSettings");
  m_settings->setValue(name, value);
  m_settings->endGroup();
}

/**
   @brief Read a persistent application value
   
   This method is used to read back persistent application settings which previously have been
   written using saveValue().
 */
QVariant Application::loadValue(const QString &name, const QVariant &defaultValue)
{
  m_settings->beginGroup("ApplicationSettings");
  QVariant result = m_settings->value(name, defaultValue);
  m_settings->endGroup();
  return result;
}

/**
   @brief Returns the default library of the application.
 */
Library *Application::defaultLibrary()
{
  return m_defaultLibrary;
}

void Application::createFactories()
{
  m_libraryFactories << new LocalLibraryFactory(this);
}

void Application::saveLibraries()
{
  if (!m_loadingLibraries) {
    m_settings->beginWriteArray("Library", m_libraries.size());
    for (int i = 0; i < m_libraries.size(); ++i) {
      m_settings->setArrayIndex(i);
      auto library = m_libraries.at(i);
      m_settings->setValue("name", library->name());
      m_settings->setValue("type", library->factory()->id());
      m_settings->setValue("directory", library->directory());
      m_settings->beginGroup("args");
      auto args = library->saveArgs();
      for (auto arg : args.keys()) {
        m_settings->setValue(arg, args.value(arg));
      }
      m_settings->endGroup();
    }
    m_settings->endArray();
  }
}

void Application::loadLibraries()
{
  m_loadingLibraries = true;
  for (auto library : m_libraries) {
    delete library;
  }
  int numLibraries = m_settings->beginReadArray("Library");
  for (int i = 0; i < numLibraries; ++i) {
    m_settings->setArrayIndex(i);
    QString name = m_settings->value("name").toString();
    QString type = m_settings->value("type").toString();
    QString directory = m_settings->value("directory").toString();
    QVariantMap args;
    m_settings->beginGroup("args");
    for (auto value : m_settings->childKeys()) {
      args["value"] = m_settings->value(value);
    }
    m_settings->endGroup();
    addLibrary(type, name, directory, args);
  }
  m_settings->endArray();
  m_loadingLibraries = false;
  
  if (!m_isInCustomLocation) {
    QString defaultLibLocation = defaultLibraryLocation();
    QFileInfo defaultLibraryLocationFI(defaultLibLocation);
    for (Library* lib : m_libraries) {
      if (QFileInfo(lib->directory()) == defaultLibraryLocationFI) {
        m_defaultLibrary = lib;
        emit defaultLibraryChanged();
        runMigrations();
        return;
      }
    }
    m_defaultLibrary = addLocalLibrary(defaultLibLocation);
    emit defaultLibraryChanged();
    runMigrations();
  }
}

Library *Application::librariesAt(QQmlListProperty<Library> *property, int index)
{
  Application *_this = qobject_cast<Application*>(property->object);
  Q_CHECK_PTR(_this);
  return _this->libraries().at(index);
}

int Application::librariesCount(QQmlListProperty<Library> *property)
{
  Application *_this = qobject_cast<Application*>(property->object);
  Q_CHECK_PTR(_this);
  return _this->libraries().size();
}

/**
   @brief Returns the location of the default library.
 */
QString Application::defaultLibraryLocation() const
{
#ifdef Q_OS_ANDROID
  QString s( qgetenv( "EXTERNAL_STORAGE" ) );
  QDir dir( s + "/data/net.rpdev.opentodolist/" );
  return dir.absolutePath();
#else
  QString result = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  result += "/Inbox/";
  return QDir(result).absolutePath();
#endif
}

void Application::runMigrations()
{
  if (m_defaultLibrary) {
    m_settings->beginGroup("Migrations");
    if (!m_settings->value("1_x_to_2_0_run", false).toBool()) {
      Migrator_1_x_to_2_x(m_defaultLibrary).migrate();
      m_settings->setValue("1_x_to_2_0_run", true);
    }
    m_settings->endGroup();
  }
}

void Application::onLibraryDeleted(Library *library)
{
  if (m_libraries.contains(library)) {
    m_libraries.removeAll(library);
  }
  saveLibraries();
  emit librariesChanged();
}


