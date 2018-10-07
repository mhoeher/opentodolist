#include "application.h"

#include <QClipboard>
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QProcess>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QTextDocument>
#include <QThreadPool>
#include <QTimer>
#include <QUuid>

#include "sync/synchronizer.h"
#include "sync/syncjob.h"
#include "sync/syncrunner.h"
#include "sync/webdavsynchronizer.h"
#include "utils/jsonutils.h"
#include "utils/keystore.h"

#include "migrators/migrator_2_x_to_3_x.h"


static Q_LOGGING_CATEGORY(log, "OpenTodoList.Application", QtDebugMsg)


/**
 * @brief Constructor.
 *
 * Creates a new Application object. The instance will be a child of the given @p parent.
 */
Application::Application(QObject *parent) :
    QObject(parent),
    m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                             QCoreApplication::organizationName(),
                             QCoreApplication::applicationName(), this)),
    m_loadingLibraries(false),
    m_keyStore(new KeyStore(this)),
    m_secrets()
{
    initialize();
}

/**
 * @brief Constructor.
 *
 * Creates a new Application object working with the given @p settings object.
 */

Application::Application(QSettings *settings, QObject *parent) :
    QObject(parent),
    m_settings(settings),
    m_loadingLibraries(false)
{
    Q_CHECK_PTR(m_settings);
    initialize();
}

/**
 * @brief Shared initialization of constructors.
 */
void Application::initialize()
{
    loadLibraries();

    connect(m_keyStore, &KeyStore::credentialsLoaded,
            [=](const QString& key, const QString& value, bool success) {
        if (success) {
            if (!m_secrets.contains(key)) {
                m_secrets.insert(key, value);
                for (auto lib : m_libraries) {
                    QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
                    if (sync) {
                        if (sync->secretsKey() == key) {
                            lib->setSecretsMissing(false);
                            qCDebug(log) << "Start-up sync of"
                                                 << lib << lib->name();
                            syncLibrary(lib);
                        }
                    }
                }
            } else {
                qCWarning(log) << "Received credentials for a key"
                                       << key << "but we already have"
                                       << "credentials for that one";
            }
        }
    });

    auto syncTimer = new QTimer(this);
    syncTimer->setInterval(1000 * 60 * 5); // Check if we need to sync every 5 min
    syncTimer->setSingleShot(false);
    connect(syncTimer, &QTimer::timeout, [=]() {
        for (auto lib : m_libraries) {
            QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
            if (sync) {
                auto lastSync = sync->lastSync();
                bool runSync = false;
                if (!lastSync.isValid()) {
                    runSync = true;
                } else {
                    auto currentDateTime = QDateTime::currentDateTime();
                    auto diff = currentDateTime.toMSecsSinceEpoch() -
                            lastSync.toMSecsSinceEpoch();
                    if (diff >= (1000 * 60 * 60)) {
                        // Sync every hour
                        qCDebug(log) << "Library" << lib
                                             << lib->name()
                                             << "has not been synced for"
                                             << "more than an hour,"
                                             << "starting sync now";
                        runSync = true;
                    }
                }
                if (runSync) {
                    syncLibrary(lib);
                }
            }
        }
    });
    syncTimer->start();
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

    if (!isLibraryDir(QUrl::fromLocalFile(localPath)) || localPath == librariesLocation()) {
        auto modLocalPath = localPath + "/" + uid.toString();
        if (QDir(modLocalPath).exists()) {
            // Do not re-use existing directory:
            localPath += "/" + QUuid::createUuid().toString();
        } else {
            localPath = modLocalPath;
        }
    }

    QDir(localPath).mkpath(".");

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
                    path = "OpenTodoList/" + uid.toString() + ".otl";
                    sync->setCreateDirs(true);
                }
                sync->setRemoteDirectory(path);
                sync->save();
                m_keyStore->saveCredentials(sync->secretsKey(), sync->secret());
                m_secrets.insert(sync->secretsKey(), sync->secret());
                delete sync;
            }
        }
        appendLibrary(result);
        syncLibrary(result);
    }
    return result;
}


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
 * @brief Converts HTML into plain text.
 *
 * This function gets an @p html string as input and returns the text converted
 * to plain text.
 */
QString Application::htmlToPlainText(const QString &html) const
{
    QTextDocument doc;
    doc.setHtml(html);
    return doc.toPlainText();
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
 * @brief Returns the home location of the current user.
 */
QUrl Application::homeLocation() const
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    return QUrl::fromLocalFile(homeDir);
}

/**
 * @brief Returns true of the folder pointed to by @p url exists.
 */
bool Application::folderExists(const QUrl &url) const
{
    return url.isValid() && QDir(url.toLocalFile()).exists();
}


/**
 * @brief Get the secrets for a synchronizer.
 *
 * Get the secrets for the synchronizer @p sync. If no secrets
 * for the synchronizer where stored, an empty string is returned.
 */
QString Application::secretForSynchronizer(Synchronizer *sync)
{
    QString result;
    if (sync != nullptr) {
        auto key = sync->secretsKey();
        if (!key.isEmpty() && m_secrets.contains(key)) {
            result = m_secrets.value(key).toString();
        }
    }
    return result;
}


/**
 * @brief Start synchronizing the @p library.
 */
void Application::syncLibrary(Library *library)
{
    if (library != nullptr) {
        if (!library->synchronizing()) {
            QScopedPointer<Synchronizer> sync(library->createSynchronizer());
            if (!sync) {
                return;
            }
            auto key = sync->secretsKey();
            QString secret;
            if (!key.isEmpty()) {
                if (m_secrets.contains(key)) {
                    secret = m_secrets.value(key).toString();
                } else {
                    qCWarning(log) << "Missing sync secret for library"
                                           << library << library->name();
                    return;
                }
            }
            library->setSynchronizing(true);
            library->clearSyncErrors();
            auto job = new SyncJob(library, library->directory(), secret);
            connect(qApp, &QCoreApplication::aboutToQuit, job, &SyncJob::stop);
            connect(job, &SyncJob::syncFinished,
                    this, &Application::onLibrarySyncFinished,
                    Qt::QueuedConnection);
            connect(job, &SyncJob::syncError,
                    library, &Library::addSyncError, Qt::QueuedConnection);
            auto runner = new SyncRunner(job);
            QThreadPool::globalInstance()->start(runner);
        }
    }
}


/**
 * @brief Save the secrets for a synchronizer.
 *
 * This saves the secrets of the given Synchronizer @p sync to the
 * key store.
 */
void Application::saveSynchronizerSecrets(Synchronizer *sync)
{
    if (sync != nullptr) {
        auto key = sync->secretsKey();
        if (!key.isEmpty()) {
            auto secret = sync->secret();
            m_keyStore->saveCredentials(key, secret);
            m_secrets[key] = secret;
            for (auto lib : m_libraries) {
                QScopedPointer<Synchronizer> s(lib->createSynchronizer());
                if (s && s->secretsKey() == key) {
                    lib->setSecretsMissing(false);
                }
            }
        }
    }
}


/**
 * @brief Copy the @p text to the clipboard.
 */
void Application::copyToClipboard(const QString &text)
{
    auto app = dynamic_cast<QGuiApplication*>(qApp);
    if (app != nullptr) {
        auto clipboard = app->clipboard();
        clipboard->setText(text);
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
    }
}

void Application::loadLibraries()
{
    m_loadingLibraries = true;
    for (auto library : m_libraries) {
        delete library;
    }
    m_secrets.clear();

    m_loadingLibraries = false;
    runMigrations();
    m_loadingLibraries = true;
    int numLibraries = m_settings->beginReadArray("LibraryDirectories");
    for (int i = 0; i < numLibraries; ++i) {
        m_settings->setArrayIndex(i);
        auto directory = m_settings->value("directory").toString();
        auto library = new Library(directory, this);
        appendLibrary(library);
        QScopedPointer<Synchronizer> sync(library->createSynchronizer());
        if (sync) {
            auto key = sync->secretsKey();
            if (!key.isEmpty()) {
                m_keyStore->loadCredentials(key);
                library->setSecretsMissing(true);
            }
        }
    }
    m_settings->endArray();
    emit librariesChanged();
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
    return defaultLibrariesLocation();
}


/**
 * @brief Get the location where libraries are stored by default.
 */
QString Application::defaultLibrariesLocation()
{
    QString result;
    result = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    result =  QDir(result).absolutePath();
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


void Application::appendLibrary(Library* library)
{
    Q_CHECK_PTR(library);
    connect(library, &Library::libraryDeleted,
            this, &Application::onLibraryDeleted);
    connect(library, &Library::deletingLibrary, [=](Library* library) {
        QScopedPointer<Synchronizer> sync(library->createSynchronizer());
        if (sync) {
            auto key = sync->secretsKey();
            if (!key.isEmpty()) {
                m_keyStore->deleteCredentials(key);
            }
        }
    });
    connect(library, &Library::needSync, [=]() {
        syncLibrary(library);
    });
    library->load();
    m_libraries.append(library);
    saveLibraries();
    emit librariesChanged();
}


void Application::onLibraryDeleted(Library *library)
{
    if (m_libraries.contains(library)) {
        m_libraries.removeAll(library);
    }
    saveLibraries();
    emit librariesChanged();
}

void Application::onLibrarySyncFinished(Library *library)
{
    for (auto lib : m_libraries) {
        if (lib == library) {
            lib->setSynchronizing(false);
            QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
            if (sync) {
                sync->setLastSync(QDateTime::currentDateTime());
                sync->save();
            }
        }
    }
}
