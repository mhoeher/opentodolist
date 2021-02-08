/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QBuffer>
#include <QDir>
#include <QDomDocument>
#include <QEventLoop>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTemporaryFile>
#include <QTimer>
#include <QtConcurrent>

#include "CompositeJob"
#include "DirectorySynchronizer"
#include "DownloadFileJob"
#include "ListFilesJob"
#include "SQLSyncStateDatabase"
#include "WebDAVGetFileInfoJob"
#include "WebDAVJobFactory"

#include "account.h"
#include "datamodel/library.h"
#include "webdavsynchronizer.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.WebDAVSynchronizer", QtDebugMsg);

const QString WebDAVSynchronizer::SyncLockFileName = ".webdav-sync-running";

static SynqClient::WebDAVServerType
toSynqClientServerType(WebDAVSynchronizer::WebDAVServerType type)
{
    switch (type) {
    case WebDAVSynchronizer::NextCloud:
        return SynqClient::WebDAVServerType::NextCloud;
    case WebDAVSynchronizer::OwnCloud:
        return SynqClient::WebDAVServerType::OwnCloud;
    case WebDAVSynchronizer::Generic:
    case WebDAVSynchronizer::Unknown:
        return SynqClient::WebDAVServerType::Generic;
    }
    return SynqClient::WebDAVServerType::Generic;
}

WebDAVSynchronizer::WebDAVSynchronizer(QObject* parent)
    : Synchronizer(parent),
      m_remoteDirectory(),
      m_disableCertificateCheck(false),
      m_username(),
      m_password(),
      m_createDirs(false),
      m_stopRequested(false),
      m_hasSyncErrors(false),
      m_nam(nullptr)
{
    connect(this, &WebDAVSynchronizer::passwordChanged, this, &WebDAVSynchronizer::secretChanged);
}

WebDAVSynchronizer::~WebDAVSynchronizer() {}

void WebDAVSynchronizer::validate()
{
    beginValidation();
    auto job = new SynqClient::WebDAVGetFileInfoJob(this);
    job->setUrl(createUrl());
    job->setUserAgent(Synchronizer::HTTPUserAgent);
    job->setNetworkAccessManager(nam());
    job->setServerType(toSynqClientServerType(m_serverType));
    job->start();
    connect(job, &SynqClient::WebDAVGetFileInfoJob::finished, this, [=]() {
        job->deleteLater();
        if (job->error() == SynqClient::JobError::NoError) {
            endValidation(true);
        } else {
            endValidation(false);
        }
    });
}

void WebDAVSynchronizer::synchronize()
{
    if (!directory().isEmpty() && !synchronizing()) {
        SynqClient::DirectorySynchronizer sync;
        sync.setRemoteDirectoryPath(m_remoteDirectory);
        sync.setLocalDirectoryPath(directory());
        sync.setSyncConflictStrategy(SynqClient::SyncConflictStrategy::RemoteWins);
        connect(&sync, &SynqClient::DirectorySynchronizer::logMessageAvailable, this,
                [=](SynqClient::SynchronizerLogEntryType type, const QString& message) {
                    switch (type) {
                    case SynqClient::SynchronizerLogEntryType::Information:
                        writeLog(Synchronizer::Debug) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::Warning:
                        writeLog(Synchronizer::Warning) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::Error:
                        writeLog(Synchronizer::Error) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::LocalMkDir:
                        writeLog(Synchronizer::LocalMkDir) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::RemoteMkDir:
                        writeLog(Synchronizer::RemoteMkDir) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::LocalDelete:
                        writeLog(Synchronizer::LocalDelete) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::RemoteDelete:
                        writeLog(Synchronizer::RemoteDelete) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::Upload:
                        writeLog(Synchronizer::Upload) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::Download:
                        writeLog(Synchronizer::Download) << message;
                        break;
                    }
                });

        SynqClient::WebDAVJobFactory factory;
        factory.setNetworkAccessManager(nam());
        factory.setServerType(toSynqClientServerType(m_serverType));
        factory.setUrl(createUrl());
        factory.setUserAgent(Synchronizer::HTTPUserAgent);
        sync.setJobFactory(&factory);

        SynqClient::SQLSyncStateDatabase db(directory() + "/.otlwebdavsync.db");
        sync.setSyncStateDatabase(&db);

        QRegularExpression pathRe(R"(^\/(library\.json|\d\d\d\d(\/\d\d?(\/[^\.]+\.[^\.]+)?)?)?$)");
        sync.setFilter([=](const QString& path, const SynqClient::FileInfo&) {
            auto result = pathRe.match(path).hasMatch();
            return result;
        });

        QEventLoop loop;
        connect(&sync, &SynqClient::DirectorySynchronizer::finished, &loop, &QEventLoop::quit);
        connect(this, &WebDAVSynchronizer::stopRequested, &sync,
                &SynqClient::DirectorySynchronizer::stop);

        setSynchronizing(true);
        m_stopRequested = false;
        m_hasSyncErrors = false;
        {
            QDir syncDir(directory());
            QFile file(syncDir.absoluteFilePath(SyncLockFileName));
            if (!file.open(QIODevice::WriteOnly)) {
                qCWarning(::log) << "Failed to create sync lock:" << file.errorString();
                error() << tr("Failed to create sync lock:") << file.errorString();
            } else {
                file.close();
            }
        }

        sync.start();
        loop.exec();

        if (sync.error() != SynqClient::SynchronizerError::NoError) {
            m_hasSyncErrors = true;
        }

        if (!m_stopRequested) {
            QDir syncDir(directory());
            syncDir.remove(SyncLockFileName);
        }
        setSynchronizing(false);
    }
}

void WebDAVSynchronizer::stopSync()
{
    qCWarning(::log) << "Stopping WebDAV sync";
    m_stopRequested = true;
    emit stopRequested();
}

/**
 * @brief Search for existing libraries.
 *
 * This method will search for existing libraries in the current
 * remote directory and the OpenTodoList sub-folder (relative to the
 * remote directory).
 */
void WebDAVSynchronizer::findExistingLibraries()
{
    if (findingLibraries()) {
        return;
    }

    setFindingLibraries(true);

    auto compositeJob = new SynqClient::CompositeJob(this);
    auto factory = new SynqClient::WebDAVJobFactory(compositeJob);
    factory->setNetworkAccessManager(nam());
    factory->setServerType(toSynqClientServerType(m_serverType));
    factory->setUrl(createUrl());
    factory->setUserAgent(Synchronizer::HTTPUserAgent);

    QSharedPointer<QVariantList> existingLibraries(new QVariantList);

    QStringList dirsToCheck { "/", "/OpenTodoList" };
    for (const auto& dir : qAsConst(dirsToCheck)) {
        auto listFilesJob = factory->listFiles(compositeJob);
        listFilesJob->setPath(m_remoteDirectory + dir);
        compositeJob->addJob(listFilesJob);
        connect(listFilesJob, &SynqClient::ListFilesJob::finished, this, [=]() {
            if (listFilesJob->error() == SynqClient::JobError::NoError) {
                auto entries = listFilesJob->entries();
                for (const auto& entry : qAsConst(entries)) {
                    if (entry.isDirectory() && entry.name().endsWith(".otl")) {
                        // The entry is a folder, most likely containing OpenTodoList data. Try to
                        // download the contained "library.json" file:
                        auto downloadJob = factory->downloadFile(compositeJob);
                        downloadJob->setRemoteFilename(listFilesJob->path() + "/" + entry.name()
                                                       + "/" + Library::LibraryFileName);
                        connect(downloadJob, &SynqClient::DownloadFileJob::finished, this, [=]() {
                            if (downloadJob->error() == SynqClient::JobError::NoError) {
                                auto doc = QJsonDocument::fromJson(downloadJob->data());
                                if (doc.isObject()) {
                                    auto map = doc.toVariant().toMap();
                                    SynchronizerExistingLibrary library;
                                    library.setName(map.value("name").toString());
                                    QDir base("/" + m_remoteDirectory);
                                    QFileInfo fi(QDir::cleanPath(base.relativeFilePath(
                                            "/" + downloadJob->remoteFilename())));
                                    library.setPath("/" + fi.path());
                                    library.setUid(map.value("uid").toUuid());
                                    qWarning() << library.path();
                                    existingLibraries->append(QVariant::fromValue(library));
                                }
                            }
                        });
                        compositeJob->addJob(downloadJob);
                    }
                }
            }
        });
    }
    connect(compositeJob, &SynqClient::CompositeJob::finished, this, [=]() {
        setExistingLibraries(*existingLibraries);
        setFindingLibraries(false);
    });

    compositeJob->start();
}

QVariantMap WebDAVSynchronizer::toMap() const
{
    auto result = Synchronizer::toMap();
    result["remoteDirectory"] = m_remoteDirectory;
    result["createDirs"] = m_createDirs;
    return result;
}

void WebDAVSynchronizer::fromMap(const QVariantMap& map)
{
    m_remoteDirectory = map.value("remoteDirectory").toString();
    m_createDirs = map.value("createDirs", false).toBool();
    Synchronizer::fromMap(map);
}

void WebDAVSynchronizer::setAccount(Account* account)
{
    m_username = account->username();
    m_password = account->password();
    m_disableCertificateCheck = account->disableCertificateChecks();
    m_url = account->baseUrl();
    switch (account->type()) {
    case Account::NextCloud:
        m_serverType = NextCloud;
        break;
    case Account::OwnCloud:
        m_serverType = OwnCloud;
        break;
    case Account::WebDAV:
        m_serverType = Generic;
        break;
    default:
        qCWarning(::log) << "Invalid account type" << account->type();
        m_serverType = Unknown;
        break;
    }
}

QString WebDAVSynchronizer::remoteDirectory() const
{
    return m_remoteDirectory;
}

void WebDAVSynchronizer::setRemoteDirectory(const QString& remoteDirectory)
{
    if (m_remoteDirectory != remoteDirectory) {
        m_remoteDirectory = remoteDirectory;
        emit remoteDirectoryChanged();
    }
}

bool WebDAVSynchronizer::disableCertificateCheck() const
{
    return m_disableCertificateCheck;
}

void WebDAVSynchronizer::setDisableCertificateCheck(bool disableCertificateCheck)
{
    if (m_disableCertificateCheck != disableCertificateCheck) {
        m_disableCertificateCheck = disableCertificateCheck;
        setValid(false);
        emit disableCertificateCheckChanged();
    }
}

QString WebDAVSynchronizer::username() const
{
    return m_username;
}

void WebDAVSynchronizer::setUsername(const QString& username)
{
    if (m_username != username) {
        m_username = username;
        setValid(false);
        emit usernameChanged();
    }
}

QString WebDAVSynchronizer::password() const
{
    return m_password;
}

void WebDAVSynchronizer::setPassword(const QString& password)
{
    if (m_password != password) {
        m_password = password;
        setValid(false);
        emit passwordChanged();
    }
}

/**
 * @brief The URL to connect to.
 */
QUrl WebDAVSynchronizer::url() const
{
    return m_url;
}

/**
 * @brief Set the server URL.
 */
void WebDAVSynchronizer::setUrl(const QUrl& url)
{
    if (m_url != url) {
        m_url = url;
        setValid(false);
        emit urlChanged();
    }
}

/**
 * @brief The type of WebDAV server to connect to.
 */
WebDAVSynchronizer::WebDAVServerType WebDAVSynchronizer::serverType() const
{
    return m_serverType;
}

void WebDAVSynchronizer::setServerType(const WebDAVServerType& serverType)
{
    if (m_serverType != serverType) {
        m_serverType = serverType;
        setValid(false);
        emit serverTypeChanged();
    }
}

bool WebDAVSynchronizer::createDirs() const
{
    return m_createDirs;
}

void WebDAVSynchronizer::setCreateDirs(bool createDirs)
{
    m_createDirs = createDirs;
}

QVariantMap WebDAVSynchronizer::toFullMap() const
{
    QVariantMap result;
    result["url"] = m_url;
    result["username"] = m_username;
    result["password"] = m_password;
    result["remoteDir"] = m_remoteDirectory;
    result["serverType"] = m_serverType;
    return result;
}

void WebDAVSynchronizer::fromFullMap(const QVariantMap& map)
{
    m_url = map["url"].toUrl();
    m_username = map["username"].toString();
    m_password = map["password"].toString();
    m_remoteDirectory = map["remoteDir"].toString();
    m_serverType = map["serverType"].value<WebDAVServerType>();
}

QNetworkAccessManager* WebDAVSynchronizer::nam()
{
    if (m_nam == nullptr) {
        m_nam = new QNetworkAccessManager(this);
        m_nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
        connect(m_nam, &QNetworkAccessManager::sslErrors, this,
                [=](QNetworkReply* reply, const QList<QSslError>& errors) {
                    if (!m_disableCertificateCheck) {
                        reply->ignoreSslErrors(errors);
                    }
                });
    }
    return m_nam;
}

QUrl WebDAVSynchronizer::createUrl() const
{
    auto result = m_url;
    result.setUserName(m_username);
    result.setPassword(m_password);
    return result;
}
