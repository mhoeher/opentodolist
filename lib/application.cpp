#include "application.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QJsonDocument>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>

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
  m_isInCustomLocation(false),
  m_updatesAvailable(false)
{
  Q_CHECK_PTR(m_settings);
  loadLibraries();

  checkForUpdates(); // Check for updates when we start....
  QTimer *timer = new QTimer(this);
  timer->setInterval(1000*60*60); // And every hour.
  connect(timer, &QTimer::timeout, [this] {
    checkForUpdates();
  });
}

Application::Application(QSettings *settings, QObject *parent) :
  QObject(parent),
  m_settings(settings),
  m_loadingLibraries(false),
  m_isInCustomLocation(true),
  m_updatesAvailable(false)
{
  Q_CHECK_PTR(m_settings);
  loadLibraries();

  checkForUpdates(); // Check for updates when we start....
  QTimer *timer = new QTimer(this);
  timer->setInterval(1000*60*60); // And every hour.
  connect(timer, &QTimer::timeout, [this] {
    checkForUpdates();
  });
}

Application::~Application()
{
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
   @brief Reads a file and returns its content
 */
QString Application::readFile(const QString &fileName) const
{
  QFile file(fileName);
  if (file.open(QIODevice::ReadOnly)) {
    return file.readAll();
  }
  return QString();
}

/**
   @brief Return a list of all 3rd party information found in the apps resource system.
 */
QVariant Application::find3rdPartyInfos() const
{
  QDirIterator it(":/", {"3rdpartyinfo.json"}, QDir::Files, QDirIterator::Subdirectories);
  QVariantList result;
  while (it.hasNext()) {
    QString file = it.next();
    QFile f(file);
    if(f.open(QIODevice::ReadOnly)) {
      QJsonParseError errorMessage;
      QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &errorMessage);
      if (errorMessage.error == QJsonParseError::NoError) {
        result.append(doc.toVariant());
      } else {
        qWarning().noquote().nospace()
            << "Failed to parse 3rd Party Info file " << file
            << ": " << errorMessage.errorString();
      }
      f.close();
    }
  }
  return result;
}

/**
   @brief Convert a URL to a local file name.
 */
QString Application::urlToLocalFile(const QUrl &url) const
{
    return url.toLocalFile();
}

/**
   @brief Convert a local file name to a url.
 */
QUrl Application::localFileToUrl(const QString &localFile) const
{
    return QUrl::fromLocalFile(localFile);
}

/**
   @brief Returns the default library of the application.
 */
Library *Application::defaultLibrary()
{
  return m_defaultLibrary;
}

/**
   @brief Are updates available?

   This returns true if updates of the app are available or false otherwise.
   Note that this functionality might not be present on all platforms, as e.g. on mobile
   ones app updates are usually done via app store installations.
 */
bool Application::updatesAvailable() const
{
  return m_updatesAvailable;
}

/**
   @brief Sets the updates available status.
 */
void Application::setUpdatesAvailable(bool updatesAvailable)
{
  if (m_updatesAvailable != updatesAvailable) {
    m_updatesAvailable = updatesAvailable;
    emit updatesAvailableChanged();
  }
}

/**
   @brief Check if updates are available

   This method checks if updates are available. If forceCheck is true, then
   this will poll the upstream server on whether a newer version of the app is available. Otherwise,
   the last update status from the config file is used unless it is too old.
 */
void Application::checkForUpdates(bool forceCheck)
{
  if (forceCheck) {
    runUpdateCheck();
  } else {
    runCachedUpdateCheck();
  }
}

void Application::runUpdate()
{
  resetUpdateCheck();
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
  QString maintenanceToolExe = QCoreApplication::applicationDirPath() +
      "/../OpenTodoListMaintenanceTool";
#ifdef Q_OS_WIN
  maintenanceToolExe += ".exe";
#endif
  if (QFile::exists(maintenanceToolExe)) {
    QProcess::startDetached(maintenanceToolExe, {"--updater"});
  }
#endif
}

bool Application::hasUpdateService() const
{
#if defined(Q_OS_LINUX)
  return QFile::exists(QCoreApplication::applicationDirPath() + "/../OpenTodoListMaintenanceTool");
#elif defined(Q_OS_WIN)
    return QFile::exists(QCoreApplication::applicationDirPath() + "/../OpenTodoListMaintenanceTool.exe");
#else
  return false;
#endif
}

/**
   @brief Returns the home location of the current user.
 */
QUrl Application::homeLocation() const
{
#ifdef Q_OS_ANDROID
  return QUrl::fromLocalFile(qgetenv("EXTERNAL_STORAGE"));
#else
  QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  return QUrl::fromLocalFile(homeDir);
#endif
}

/**
   @brief Returns true of the folder pointed to by @p url exists.
 */
bool Application::folderExists(const QUrl &url) const
{
  return url.isValid() && QDir(url.toLocalFile()).exists();
}

void Application::saveLibraries()
{
  /*if (!m_loadingLibraries) {
    m_settings->beginWriteArray("Library", m_libraries.size());
    for (int i = 0; i < m_libraries.size(); ++i) {
      m_settings->setArrayIndex(i);
      auto library = m_libraries.at(i);
      m_settings->setValue("name", library->name());
      m_settings->setValue("directory", library->directory());
      m_settings->setValue("tags", library->tags());
      m_settings->beginGroup("args");
      auto args = library->saveArgs();
      for (auto arg : args.keys()) {
        m_settings->setValue(arg, args.value(arg));
      }
      m_settings->endGroup();
    }
    m_settings->endArray();
  }*/
}

void Application::loadLibraries()
{
  m_loadingLibraries = true;
  for (auto library : m_libraries) {
    delete library;
  }
  /*
  int numLibraries = m_settings->beginReadArray("Library");
  for (int i = 0; i < numLibraries; ++i) {
    m_settings->setArrayIndex(i);
    QString name = m_settings->value("name").toString();
    QString type = m_settings->value("type").toString();
    QString directory = m_settings->value("directory").toString();
    QStringList tags = m_settings->value("tags", QStringList()).toStringList();
    QVariantMap args;
    m_settings->beginGroup("args");
    for (auto value : m_settings->childKeys()) {
      args["value"] = m_settings->value(value);
    }
    m_settings->endGroup();
    auto library = addLibrary(type, name, directory, args);
    if (library) {
        library->setTags(tags);
    }
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
  }*/
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
  QDir dir( s + "/data/net.rpdev.opentodolist/Inbox/" );
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
      //Migrator_1_x_to_2_x(m_defaultLibrary).migrate();
      //m_settings->setValue("1_x_to_2_0_run", true);
    }
    m_settings->endGroup();
  }
}

void Application::runUpdateCheck()
{
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
  QString maintenanceToolExe = QCoreApplication::applicationDirPath() +
      "/../OpenTodoListMaintenanceTool";
#ifdef Q_OS_WIN
  maintenanceToolExe += ".exe";
#endif
  if (QFile::exists(maintenanceToolExe)) {
    QProcess *maintenanceTool = new QProcess(this);
    Q_CHECK_PTR(maintenanceTool);
    maintenanceTool->setProgram(maintenanceToolExe);
    maintenanceTool->setArguments({"--checkupdates"});
    connect(maintenanceTool, &QProcess::stateChanged,
            [this, maintenanceTool](QProcess::ProcessState newState) {
      if (newState == QProcess::NotRunning) {
        saveUpdatesAvailable(maintenanceTool->exitCode() == 0);
        maintenanceTool->deleteLater();
      }
    });
    maintenanceTool->start();
  }
#endif
}

void Application::resetUpdateCheck()
{
    m_settings->beginGroup("Updates");
    m_settings->setValue("updatesAvailable", false);
    setUpdatesAvailable(false);
    m_settings->endGroup();
}

void Application::runCachedUpdateCheck()
{
  m_settings->beginGroup("Updates");
  QDateTime lastCheck = m_settings->value("lastCheck").toDateTime();
  bool hasUpdates = m_settings->value("updatesAvailable", false).toBool();
  m_settings->endGroup();

  setUpdatesAvailable(hasUpdates);
  if (!lastCheck.isValid() || lastCheck.daysTo(QDateTime::currentDateTime()) > 1) {
    runUpdateCheck();
  }
}

void Application::saveUpdatesAvailable(bool available)
{
  m_settings->beginGroup("Updates");
  m_settings->setValue("lastCheck", QDateTime::currentDateTime());
  m_settings->setValue("updatesAvailable", available);
  m_settings->endGroup();
  setUpdatesAvailable(available);
}

void Application::onLibraryDeleted(Library *library)
{
  if (m_libraries.contains(library)) {
    m_libraries.removeAll(library);
  }
  saveLibraries();
  emit librariesChanged();
}


