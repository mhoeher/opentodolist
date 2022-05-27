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

#include "SynqClient/CompositeJob"
#include "SynqClient/DirectorySynchronizer"
#include "SynqClient/DownloadFileJob"
#include "SynqClient/ListFilesJob"
#include "SynqClient/SQLSyncStateDatabase"
#include "SynqClient/WebDAVGetFileInfoJob"
#include "SynqClient/WebDAVJobFactory"

#include "account.h"
#include "datamodel/library.h"
#include "webdavaccount.h"
#include "webdavsynchronizer.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.WebDAVSynchronizer", QtDebugMsg);

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
      m_disableCertificateCheck(false),
      m_workarounds(0),
      m_username(),
      m_password(),
      m_stopRequested(false),
      m_hasSyncErrors(false),
      m_nam(nullptr)
{
    connect(this, &WebDAVSynchronizer::passwordChanged, this, &WebDAVSynchronizer::secretChanged);
}

WebDAVSynchronizer::~WebDAVSynchronizer() {}

void WebDAVSynchronizer::setupFactory(SynqClient::WebDAVJobFactory& factory)
{
    factory.setNetworkAccessManager(nam());
    factory.setServerType(toSynqClientServerType(m_serverType));
    factory.setUrl(createUrl());
    factory.setUserAgent(Synchronizer::HTTPUserAgent);
    factory.setWorkarounds(static_cast<SynqClient::WebDAVWorkarounds>(m_workarounds));
}

void WebDAVSynchronizer::synchronize()
{
    bool retryWithOneJobOnly = false;
    bool finished = false;
    while (!finished) {
        finished = true;
        if (!directory().isEmpty() && !synchronizing()) {
            SynqClient::DirectorySynchronizer sync;
            sync.setRemoteDirectoryPath(remoteDirectory());
            sync.setLocalDirectoryPath(directory());
            if (retryWithOneJobOnly) {
                // WA for https://github.com/mhoeher/opentodolist/issues/46
                sync.setMaxJobs(1);
            }
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
            connect(&sync, &SynqClient::DirectorySynchronizer::progress, this,
                    &WebDAVSynchronizer::progress);

            SynqClient::WebDAVJobFactory factory;
            setupFactory(factory);
            sync.setJobFactory(&factory);

            SynqClient::SQLSyncStateDatabase db(directory() + "/.otlwebdavsync.db");
            sync.setSyncStateDatabase(&db);

            static QRegularExpression pathRe(
                    R"(^\/(library\.json|\d\d\d\d(\/\d\d?(\/[^\.]+\.[a-zA-Z\.]+)?)?)?$)");
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

            sync.start();
            loop.exec();

            if (sync.error() != SynqClient::SynchronizerError::NoError) {
                m_hasSyncErrors = true;
                emit syncError(sync.errorString());
                if (sync.retryWithFewerJobs() && !retryWithOneJobOnly) {
                    retryWithOneJobOnly = true;
                    finished = false;
                }
            }

            setSynchronizing(false);
        }
    }
}

void WebDAVSynchronizer::stopSync()
{
    qCWarning(::log) << "Stopping WebDAV sync";
    m_stopRequested = true;
    emit stopRequested();
}

void WebDAVSynchronizer::setAccount(Account* account)
{
    auto webDavAccount = qobject_cast<WebDAVAccount*>(account);
    q_check_ptr(webDavAccount);
    m_username = webDavAccount->username();
    m_password = webDavAccount->password();
    m_disableCertificateCheck = webDavAccount->disableCertificateChecks();
    m_workarounds = webDavAccount->backendSpecificData().value("workarounds", 0).toInt();
    m_url = webDavAccount->baseUrl();
    switch (webDavAccount->type()) {
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

bool WebDAVSynchronizer::disableCertificateCheck() const
{
    return m_disableCertificateCheck;
}

void WebDAVSynchronizer::setDisableCertificateCheck(bool disableCertificateCheck)
{
    if (m_disableCertificateCheck != disableCertificateCheck) {
        m_disableCertificateCheck = disableCertificateCheck;
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
        emit serverTypeChanged();
    }
}

int WebDAVSynchronizer::workarounds() const
{
    return m_workarounds;
}

void WebDAVSynchronizer::setWorkarounds(int newWorkarounds)
{
    if (m_workarounds == newWorkarounds)
        return;
    m_workarounds = newWorkarounds;
    emit workaroundsChanged();
}

QVariantMap WebDAVSynchronizer::toFullMap() const
{
    QVariantMap result;
    result["url"] = m_url;
    result["username"] = m_username;
    result["password"] = m_password;
    result["remoteDir"] = remoteDirectory();
    result["serverType"] = m_serverType;
    return result;
}

void WebDAVSynchronizer::fromFullMap(const QVariantMap& map)
{
    m_url = map["url"].toUrl();
    m_username = map["username"].toString();
    m_password = map["password"].toString();
    setRemoteDirectory(map["remoteDir"].toString());
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
