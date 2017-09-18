#include "application.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QJsonDocument>
#include <QProcess>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QTimer>
#include <QUuid>

#include "utils/jsonutils.h"
#include "utils/keystore.h"
#include "sync/synchronizer.h"
#include "sync/webdavsynchronizer.h"

#include "migrators/migrator_2_x_to_3_x.h"


/**
 * @brief Constructor.
 *
 * Creates a new Application object. The instance will be a child of the given @p parent.
 */
Application::Application(QObject *parent) :
    QObject(parent),
    m_defaultLibrary(nullptr),
    m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                             QCoreApplication::organizationName(),
                             QCoreApplication::applicationName(), this)),
    m_loadingLibraries(false),
    m_updatesAvailable(false),
    m_keyStore(new KeyStore(this)),
    m_secrets()
{
    loadLibraries();

    checkForUpdates(); // Check for updates when we start....
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000*60*60); // And every hour.
    connect(timer, &QTimer::timeout, [this] {
        checkForUpdates();
    });
    loadSecrets();
}

/**
 * @brief Constructor.
 *
 * Creates a new Application object working with the given @p settings object.
 */
Application::Application(QSettings *settings, QObject *parent) :
    QObject(parent),
    m_defaultLibrary(nullptr),
    m_settings(settings),
    m_loadingLibraries(false),
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
    loadSecrets();
}

/**
 * @brief Destructor.
 */
Application::~Application()
{
}

/**
 * @brief Returns the libraries as a QML list property.
 */
QQmlListProperty<Library> Application::libraryList()
{
    return QQmlListProperty<Library>(this, nullptr, librariesCount, librariesAt);
}


/**
 * @brief Create a new library.
 *
 * This creates a new library using the given @p parameters. The following
 * keys are assumed to be provided (some of them optional):
 *
 * - name: The name of the library.
 * - localPath: The path where to create the library in.
 * - synchronizer: The type (class name) of synchronizer to use.
 * - serverType: For some synchronizer classes, the concrete sub-type of the server.
 * - uid: The UID to use for the library.
 * - path: The remote path of the library.
 *
 * If no synchronizer is specified and localPath points to a location
 * with an existing library, that library will be imported. In this
 * case, any other attributes are ignored.
 */
Library *Application::addLibrary(const QVariantMap &parameters)
{
    Library* result = nullptr;

    auto synchronizerCls = parameters.value("synchronizer").toString();
    auto localPath = parameters.value("localPath", librariesLocation()).toString();
    auto name = parameters.value("name").toString();
    QUuid uid;
    if (parameters.contains("uid") && parameters.value("uid").toString() != "") {
        uid = parameters.value("uid").toUuid();
    } else {
        uid = QUuid::createUuid();
    }
    auto path = parameters.value("path").toString();
    if (synchronizerCls.isEmpty() || path.isEmpty()) {
        if (!isLibraryDir(localPath)) {
            localPath += "/" + uid.toString();
        }
    }

    QDir(localPath).mkpath(".");

    if (isLibraryDir(localPath)) {

    }
    if (QDir(localPath).exists()) {
        result = new Library(localPath, this);
        result->setUid(uid);
        result->setName(name);
        if (!synchronizerCls.isEmpty()) {
            if (synchronizerCls == "WebDAVSynchronizer") {
                auto sync = new WebDAVSynchronizer();
                auto type = parameters.value("serverType")
                        .value<WebDAVSynchronizer::WebDAVServerType>();
                sync->setServerType(type);
                sync->setUrl(parameters.value("url").toUrl());
                sync->setUsername(parameters.value("username").toString());
                sync->setPassword(parameters.value("password").toString());
                sync->setDisableCertificateCheck(
                            parameters.value(
                                "disableCertificateCheck").toBool());
                sync->setDirectory(result->directory());
                if (path.isEmpty()) {
                    path = "OpenTodoList/" + uid.toString();
                    sync->setCreateDirs(true);
                }
                sync->setRemoteDirectory(path);
                sync->save();
                setSecret(result, sync->password());
                delete sync;
            }
        }
    }
    appendLibrary(result);
    return result;
}

/**
 * @brief Add a new library.
 *
 * This creates a new library and returns it. If @p url points to an existing
 * directory, the library will use it for storing its data. If the url is invalid,
 * the library will be created in the default library location.
 */
/*Library*Application::addLibrary(const QUrl& url)
{
    Library* result = nullptr;
    auto path = url.toLocalFile();
    QDir dir(path);
    if (url.isValid() && dir.exists()) {
        result = new Library(path, this);
        if (!result->load()) { // Is this an existing library?
            result->save(); // If not, save immediately to preserve data.
        }
        appendLibrary(result);
    } else {
        auto uid = QUuid::createUuid();
        path = librariesLocation() + "/" + uid.toString();
        QDir(path).mkpath(".");
        result = new Library(path, this);
        result->setUid(uid);
        appendLibrary(result);
    }
    return result;
}
*/
/**
 * @brief Save a value to the application settings
 *
 * This method is used to save a value to the application settings. Settings can be restored
 * (e.g. when the app restarts).
 */
void Application::saveValue(const QString &name, const QVariant &value)
{
    m_settings->beginGroup("ApplicationSettings");
    m_settings->setValue(name, value);
    m_settings->endGroup();
}

/**
 * @brief Read a persistent application value
 *
 * This method is used to read back persistent application settings which previously have been
 * written using saveValue().
 */
QVariant Application::loadValue(const QString &name, const QVariant &defaultValue)
{
    m_settings->beginGroup("ApplicationSettings");
    QVariant result = m_settings->value(name, defaultValue);
    m_settings->endGroup();
    return result;
}

/**
 * @brief Reads a file and returns its content
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
 * @brief Return a list of all 3rd party information found in the apps resource system.
 */
QVariant Application::find3rdPartyInfos() const
{
    QDirIterator it(":/", {"3rdpartyinfo.json"}, QDir::Files, QDirIterator::Subdirectories);
    QVariantList result;
    while (it.hasNext()) {
        QString file = it.next();
        bool ok;
        auto map = JsonUtils::loadMap(file, &ok);
        if (ok) {
            result.append(map);
        }
    }
    return result;
}

/**
 * @brief Convert a URL to a local file name.
 */
QString Application::urlToLocalFile(const QUrl &url) const
{
    return url.toLocalFile();
}

/**
 * @brief Convert a local file name to a url.
 */
QUrl Application::localFileToUrl(const QString &localFile) const
{
    return QUrl::fromLocalFile(localFile);
}


/**
 * @brief Clean the local file path given via the @p url.
 *
 * This method removes extra dots and dotdots from the file path
 * and returns a simplified verion of the input path.
 */
QUrl Application::cleanPath(const QUrl &url) const
{
    auto path = url.toLocalFile();
    path = QDir::cleanPath(path);
    return QUrl::fromLocalFile(path);
}

/**
 * @brief Check if a file called @p filename exists.
 */
bool Application::fileExists(const QString& filename) const
{
    return QFile(filename).exists();
}

/**
 * @brief Check if the @p directory exists.
 */
bool Application::directoryExists(const QString& directory) const
{
    return !directory.isEmpty() && QDir(directory).exists();
}

/**
 * @brief Get the basename of the @p filename.
 */
QString Application::basename(const QString& filename) const
{
    return QFileInfo(filename).baseName();
}


/**
 * @brief Test if the @p url points to an existing library directory.
 */
bool Application::isLibraryDir(const QUrl &url) const
{
    bool result = false;
    if (url.isValid()) {
        auto dir = QDir(url.toLocalFile());
        if (dir.exists() && dir.exists(Library::LibraryFileName)) {
            result = true;
        }
    }
    return result;
}

/**
 * @brief Returns the default library of the application.
 */
Library *Application::defaultLibrary()
{
    return m_defaultLibrary;
}

/**
 * @brief Are updates available?
 *
 * This returns true if updates of the app are available or false otherwise.
 * Note that this functionality might not be present on all platforms, as e.g. on mobile
 * ones app updates are usually done via app store installations.
 */
bool Application::updatesAvailable() const
{
    return m_updatesAvailable;
}

/**
 * @brief Sets the updates available status.
 */
void Application::setUpdatesAvailable(bool updatesAvailable)
{
    if (m_updatesAvailable != updatesAvailable) {
        m_updatesAvailable = updatesAvailable;
        emit updatesAvailableChanged();
    }
}

/**
 * @brief Check if updates are available
 *
 * This method checks if updates are available. If forceCheck is true, then
 * this will poll the upstream server on whether a newer version of the app is available. Otherwise,
 * the last update status from the config file is used unless it is too old.
 */
void Application::checkForUpdates(bool forceCheck)
{
    if (forceCheck) {
        runUpdateCheck();
    } else {
        runCachedUpdateCheck();
    }
}

/**
 * @brief Run the application updater.
 */
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

/**
 * @brief Check if an updater service is available.
 */
bool Application::hasUpdateService() const
{
#if defined(Q_OS_LINUX)
    return QFile::exists(QCoreApplication::applicationDirPath() +
                         "/../OpenTodoListMaintenanceTool");
#elif defined(Q_OS_WIN)
    return QFile::exists(QCoreApplication::applicationDirPath() +
                         "/../OpenTodoListMaintenanceTool.exe");
#else
    return false;
#endif
}

/**
 * @brief Returns the home location of the current user.
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
 * @brief Returns true of the folder pointed to by @p url exists.
 */
bool Application::folderExists(const QUrl &url) const
{
    return url.isValid() && QDir(url.toLocalFile()).exists();
}


/**
 * @brief Get the secret for the given @p library.
 */
QString Application::getSecret(Library *library)
{
    Q_CHECK_PTR(library);
    QScopedPointer<Synchronizer> sync(
                Synchronizer::fromDirectory(library->directory()));
    QString result;
    if (sync) {
        auto key = sync->secretsKey();
        result = m_secrets.value(key, QString()).toString();
    }
    return result;
}


/**
 * @brief Set a @p secret for the @p library.
 *
 * This sets the secret for the given library and stores the secret in a
 * platform specific secrets store.
 */
void Application::setSecret(Library *library, const QString &secret)
{
    Q_CHECK_PTR(library);
    QScopedPointer<Synchronizer> sync(
                Synchronizer::fromDirectory(library->directory()));
    if (sync) {
        auto key = sync->secretsKey();
        if (key != "") {
            if (!m_secrets.contains(key) || m_secrets[key] != secret) {
                m_secrets[key] = secret;
                auto json = QJsonDocument::fromVariant(m_secrets).toJson();
                m_keyStore->saveCredentials("OpenTodoList", json);
                m_settings->beginGroup("Secrets");
                m_settings->setValue("HaveSecrets", true);
                m_settings->endGroup();
            }
        }
    }
}

void Application::saveLibraries()
{
    if (!m_loadingLibraries) {
        m_settings->beginWriteArray("LibraryDirectories", m_libraries.length());
        for (int i = 0; i < m_libraries.length(); ++i) {
            auto library = m_libraries.at(i);
            m_settings->setArrayIndex(i);
            m_settings->setValue("directory", library->directory());
        }
        m_settings->endArray();
        m_settings->beginGroup("DefaultLibrary");
        m_settings->setValue("defaultLibrary", m_libraries.indexOf(m_defaultLibrary));
        m_settings->endGroup();
    }
}

void Application::loadLibraries()
{
    m_loadingLibraries = true;
    for (auto library : m_libraries) {
        delete library;
    }
    m_loadingLibraries = false;
    runMigrations();
    m_loadingLibraries = true;
    int numLibraries = m_settings->beginReadArray("LibraryDirectories");
    for (int i = 0; i < numLibraries; ++i) {
        m_settings->setArrayIndex(i);
        auto directory = m_settings->value("directory").toString();
        auto library = new Library(directory, this);
        appendLibrary(library);
    }
    m_settings->endArray();
    m_settings->beginGroup("DefaultLibrary");
    auto defaultLibraryIndex = m_settings->value("defaultLibrary", -1).toInt();
    if (defaultLibraryIndex >= 0 && defaultLibraryIndex < m_libraries.length()) {
        m_defaultLibrary = m_libraries.at(defaultLibraryIndex);
        emit defaultLibraryChanged();
    }
    m_settings->endGroup();
    emit librariesChanged();
    if (!m_defaultLibrary) {
        if (m_libraries.length() > 0) {
            m_defaultLibrary = m_libraries.at(0);
        } else {
            QDir libraryDir(defaultLibraryLocation());
            if (libraryDir.mkpath(".")) {
                m_defaultLibrary = new Library(libraryDir.absolutePath());
                m_defaultLibrary->setName(tr("Inbox"));
                emit defaultLibraryChanged();
                m_libraries.append(m_defaultLibrary);
                emit librariesChanged();
            }
        }
    }
    m_loadingLibraries = false;
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
 * @brief Get the location where libraries are stored by default.
 */
QString Application::librariesLocation() const
{
    QString result;
#ifdef Q_OS_ANDROID
    QString s(qgetenv("EXTERNAL_STORAGE"));
    QDir dir(s + "/data/net.rpdev.opentodolist/");
    result = dir.absolutePath();
#else
    result = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    result =  QDir(result).absolutePath();
#endif
    QDir(result).mkpath(".");
    return result;
}

/**
   @brief Returns the location of the default library.
 */
QString Application::defaultLibraryLocation() const
{
    return QDir(librariesLocation()).absoluteFilePath("Inbox");
}

void Application::runMigrations()
{
    m_settings->beginGroup("Migrations");
    if (!m_settings->value("2_x_to_3_0_run", false).toBool()) {
        m_settings->endGroup();
        Migrator_2_x_to_3_x migrator;
        migrator.run(this);
        m_settings->beginGroup("Migrations");
        m_settings->setValue("2_x_to_3_0_run", true);
        QTimer::singleShot(0, this, &Application::saveLibraries);
    }
    m_settings->endGroup();
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

void Application::appendLibrary(Library* library)
{
    Q_CHECK_PTR(library);
    connect(library, &Library::libraryDeleted, this, &Application::onLibraryDeleted);
    library->load();
    m_libraries.append(library);
    saveLibraries();
    emit librariesChanged();
}

void Application::loadSecrets()
{
    m_settings->beginGroup("Secrets");
    auto haveSecrets = m_settings->value("HaveSecrets", false).toBool();
    m_settings->endGroup();
    if (haveSecrets) {
        auto result = new LoadCredentialsResult(this);
        connect(result, &LoadCredentialsResult::done, [=](const QByteArray &value, bool hasError, const QString& errorString) {
            if (hasError) {
                qWarning() << "Failed to read secrets:" << errorString;
            } else {
                m_secrets = QJsonDocument::fromJson(value).toVariant().toMap();
            }
            result->deleteLater();
        });
    }
}

void Application::onLibraryDeleted(Library *library)
{
    if (m_libraries.contains(library)) {
        m_libraries.removeAll(library);
    }
    if (m_defaultLibrary == library) {
        if (m_libraries.length() > 0) {
            m_defaultLibrary = m_libraries.at(0);
        } else {
            QDir dir(defaultLibraryLocation());
            dir.mkpath(".");
            m_defaultLibrary = new Library(dir.absolutePath());
            m_libraries.append(m_defaultLibrary);
            emit defaultLibraryChanged();
        }
    }
    saveLibraries();
    emit librariesChanged();
}
