/*
 * Copyright 2022 Martin Hoeher <martin@rpdev.net>
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

#include "dropboxaccount.h"

#include <QVariant>
#include <QVariantMap>
#include <QJsonDocument>
#include <QOAuth2AuthorizationCodeFlow>
#include <QDesktopServices>
#include <QOAuthHttpServerReplyHandler>
#include <QLoggingCategory>
#include <QNetworkReply>

#include <SynqClient/CompositeJob>
#include <SynqClient/ListFilesJob>
#include <SynqClient/DropboxListFilesJob>
#include <SynqClient/DownloadFileJob>
#include <SynqClient/DropboxJobFactory>

#include "dropboxsynchronizer.h"

#include "datamodel/library.h"

static const char* RefreshToken = "refreshToken";
static const char* AccessToken = "accessToken";
static const char* CodeVerifier = "codeVerifier";
static const char* Expiration = "expiration";

static const char* DropboxAppKey = "0m6k0evdacf3mk8";
static const char* DropboxAppSecret = "j410cact7mooota";

static const char* DropboxAccessTokenUrl = "https://api.dropboxapi.com/oauth2/token";
static const char* DropboxAuthorizationUrl = "https://www.dropbox.com/oauth2/authorize";
static const char* DropboxRedirectUri = "http://localhost:1220";
static const quint16 DropboxRedirectPort = 1220;

static Q_LOGGING_CATEGORY(log, "OpenTodoList.DropboxAccount", QtDebugMsg);

DropboxAccount::DropboxAccount(QObject* parent)
    : Account { parent }, m_refreshToken(), m_accessToken(), m_expiration(), m_codeVerifier()
{
    setType(Dropbox);
}

const QString& DropboxAccount::accessToken() const
{
    return m_accessToken;
}

/**
 * @brief Verify the validity of the tokens.
 *
 * Start verifying the tokens we have stored. This eventually emits the tokensVerified() signal.
 * If the outcome is that the tokens stored are no longer valid, the user has to grant access
 * again via the oAuth flow.
 */
void DropboxAccount::verifyTokens() {}

void DropboxAccount::setAccessToken(const QString& newAccessToken)
{
    if (m_accessToken == newAccessToken)
        return;
    m_accessToken = newAccessToken;
    emit accessTokenChanged();
}

/**
 * @brief Creates a code verifier.
 *
 * This initializes the code verifier as described in
 * https://datatracker.ietf.org/doc/html/rfc7636#section-4.1.
 */
QString DropboxAccount::createCodeVerifier()
{
    auto rng = QRandomGenerator::global();
    quint32 randomData[8];
    rng->fillRange(randomData);
    QByteArray data(reinterpret_cast<const char*>(&randomData), sizeof(randomData));
    return QString(data.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}

/**
 * @brief Create a code challenge from the code verifier.
 *
 * This creates a code challenge from the code verifier as described in
 * https://datatracker.ietf.org/doc/html/rfc7636#section-4.2 using the S256 method.
 */
QString DropboxAccount::getCodeChallenge() const
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(m_codeVerifier.toUtf8());
    return hash.result().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}

SynqClient::DropboxJobFactory* DropboxAccount::createDropboxJobFactory(QObject* parent) const
{
    auto result = new SynqClient::DropboxJobFactory(parent);
    result->setNetworkAccessManager(new QNetworkAccessManager(result));
    result->setToken(m_accessToken);
    result->setUserAgent(Synchronizer::HTTPUserAgent);
    return result;
}

/**
 * @brief Create an OAuth 2 authorization code flow.
 *
 * This creates an OAuth 2 authorization code flow object suitable to gain access to the
 * user's data. If present, this will also pre-populate the flow with the refresh token
 * we received earlier.
 */
QOAuth2AuthorizationCodeFlow* DropboxAccount::createOAuthAuthFlow(QObject* parent) const
{
    auto nam = new QNetworkAccessManager();
    auto result = new QOAuth2AuthorizationCodeFlow(nam, parent);
    nam->setParent(result);
    nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    auto replyHandler = new QOAuthHttpServerReplyHandler(DropboxRedirectPort, result);
    connect(replyHandler, &QOAuthHttpServerReplyHandler::callbackDataReceived, this,
            [&](const auto& data) { qWarning() << data; });
    result->setReplyHandler(replyHandler);
    result->setModifyParametersFunction(
            [=](QOAuth2AuthorizationCodeFlow::Stage stage, QVariantMap* parameters) {
                switch (stage) {
                case QOAuth2AuthorizationCodeFlow::Stage::RequestingAuthorization:
                    // Include code challenge and method in auth request, see
                    // https://datatracker.ietf.org/doc/html/rfc7636#section-4.3:
                    parameters->insert("code_challenge", getCodeChallenge());
                    parameters->insert("code_challenge_method", "S256");
                    parameters->insert("redirect_uri", DropboxRedirectUri);
                    parameters->insert("token_access_type", "offline");
#ifdef Q_OS_IOS
                    // On iOS, disable signup and instead display a link to the iOS app store
                    // this is for compliance with signup restrictions).
                    parameters->insert("disable_signup", "true");
#endif
                    break;
                case QOAuth2AuthorizationCodeFlow::Stage::RequestingAccessToken:
                    parameters->insert("code_verifier", m_codeVerifier);
                    parameters->insert("redirect_uri", DropboxRedirectUri);
                    break;
                case QOAuth2AuthorizationCodeFlow::Stage::RefreshingAccessToken:
                    parameters->remove("redirect_uri");
                    parameters->insert("client_id", DropboxAppKey);
                    parameters->insert("client_secret", DropboxAppSecret);
                    break;
                default:
                    break;
                }
            });
    result->setAccessTokenUrl(QUrl(DropboxAccessTokenUrl));
    result->setAuthorizationUrl(QUrl(DropboxAuthorizationUrl));
    result->setRefreshToken(m_refreshToken);
    result->setToken(m_accessToken);
    result->setClientIdentifier(DropboxAppKey);
    result->setUserAgent(Synchronizer::HTTPUserAgent);
    return result;
}

void DropboxAccount::save(QSettings* settings)
{
    Account::save(settings);
}

void DropboxAccount::load(QSettings* settings)
{
    Account::load(settings);
}

QString DropboxAccount::accountSecrets() const
{
    QVariantMap doc {
        { RefreshToken, m_refreshToken },
        { AccessToken, m_accessToken },
        { CodeVerifier, m_codeVerifier },
        { Expiration, m_expiration },
    };
    return QJsonDocument::fromVariant(doc).toJson();
}

void DropboxAccount::setAccountSecrets(const QString& secrets)
{
    auto doc = QJsonDocument::fromJson(secrets.toUtf8()).toVariant().toMap();
    m_refreshToken = doc.value(RefreshToken, m_refreshToken).toString();
    m_accessToken = doc.value(AccessToken, m_accessToken).toString();
    m_codeVerifier = doc.value(CodeVerifier, m_codeVerifier).toString();
    m_expiration = doc.value(Expiration, m_expiration).toDateTime();
}

Synchronizer* DropboxAccount::createSynchronizer() const
{
    auto result = new DropboxSynchronizer();
    result->setAccessToken(m_accessToken);
    return result;
}

void DropboxAccount::login()
{
    if (loggingIn()) {
        return;
    }

    setOnline(false);
    setLoggingIn(true);

    auto oauth = createOAuthAuthFlow(this);
    connect(oauth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, oauth,
            &QDesktopServices::openUrl);
    connect(oauth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, this,
            &DropboxAccount::authorizationUrlReceived);
    connect(oauth, &QOAuth2AuthorizationCodeFlow::granted, this, [=]() {
        qCDebug(log) << "Access has been granted";
        oauth->deleteLater();
        m_refreshToken = oauth->refreshToken();
        m_accessToken = oauth->token();
        m_expiration = oauth->expirationAt();
        setOnline(true);
        setLoggingIn(false);
        emit loginFinished(true);
    });
    connect(oauth, &QOAuth2AuthorizationCodeFlow::requestFailed, this,
            [=](const QAbstractOAuth::Error& error) {
                qCDebug(log) << "Error during OAuth flow:" << static_cast<int>(error);
            });
    connect(oauth, &QOAuth2AuthorizationCodeFlow::error, this,
            [=](const QString& error, const QString& errorDescription, const QUrl& url) {
                qCWarning(log) << "oAuth error:" << error << ":" << errorDescription << url;

                if (oauth->property("refreshingTokens").toBool()) {
                    // We tried to refresh existing tokens which failes - we need to log
                    // in again:
                    oauth->setProperty("refreshingTokens", false);
                    m_codeVerifier = createCodeVerifier();
                    oauth->grant();
                } else {
                    oauth->deleteLater();
                    QString errorUrl;
                    if (url.isValid()) {
                        errorUrl = QString("<br/><br/><a href='%1'>%1</a>").arg(url.toString());
                    }
                    emit loginError(QString("<strong>%1</strong><br/><br/>%2%3")
                                            .arg(error, errorDescription, errorUrl));
                    setLoggingIn(false);
                    setOnline(false);
                    emit loginFinished(false);
                }
            });
    if (m_refreshToken.isEmpty()) {
        qCDebug(log) << "Starting grant";
        m_codeVerifier = createCodeVerifier();
        oauth->setProperty("refreshingTokens", false);
        oauth->grant();
    } else {
        qCDebug(log) << "Starting refresh";
        oauth->setProperty("refreshingTokens", true);
        oauth->refreshAccessToken();
    }
}

void DropboxAccount::findExistingLibraries()
{
    if (findingRemoteLibraries()) {
        return;
    }

    setFindingRemoteLibraries(true);

    auto compositeJob = new SynqClient::CompositeJob(this);
    auto factory = createDropboxJobFactory(compositeJob);

    QSharedPointer<QList<RemoteLibraryInfo>> existingLibraries(new QList<RemoteLibraryInfo>);

    QStringList dirsToCheck { "/OpenTodoList" };
    for (const auto& dir : qAsConst(dirsToCheck)) {
        auto listFilesJob = factory->listFiles(compositeJob);
        listFilesJob->setPath(dir);
        compositeJob->addJob(listFilesJob);
        connect(listFilesJob, &SynqClient::ListFilesJob::finished, this, [=]() {
            if (listFilesJob->error() == SynqClient::JobError::NoError) {
                auto entries = listFilesJob->entries();
                for (const auto& entry : qAsConst(entries)) {
                    if (entry.isDirectory() && entry.name().endsWith(".otl")) {
                        // The entry is a folder, most likely containing OpenTodoList data. Try
                        // to download the contained "library.json" file:
                        auto downloadJob = factory->downloadFile(compositeJob);
                        downloadJob->setRemoteFilename(listFilesJob->path() + "/" + entry.name()
                                                       + "/" + Library::LibraryFileName);
                        connect(downloadJob, &SynqClient::DownloadFileJob::finished, this, [=]() {
                            if (downloadJob->error() == SynqClient::JobError::NoError) {
                                auto doc = QJsonDocument::fromJson(downloadJob->data());
                                if (doc.isObject()) {
                                    auto map = doc.toVariant().toMap();
                                    RemoteLibraryInfo library;
                                    library.setName(map.value("name").toString());
                                    QFileInfo fi(QDir::cleanPath(downloadJob->remoteFilename()));
                                    library.setPath(fi.path());
                                    library.setUid(map.value("uid").toUuid());
                                    existingLibraries->append(library);
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
        setRemoteLibraries(*existingLibraries);
        setFindingRemoteLibraries(false);
        compositeJob->deleteLater();
    });

    compositeJob->start();
}

/**
 * @brief Implementation of Account::checkConnectivity().
 */
void DropboxAccount::checkConnectivity()
{
    setOnline(false);

    // If we have no refresh token - don't even try anything:
    if (m_refreshToken.isEmpty()) {
        emit connectivityCheckFinished(false);
        return;
    }

    // If the access token is still valid, just check if we can reach the server:
    if (QDateTime::currentDateTime().secsTo(m_expiration) > 60 * 60) {
        // TODO: Check is not working - we cannot list the remote root folder.
        // Find something else...
        qCDebug(log) << "Access token of" << uid() << "is recent enough - try to connect with it.";
        auto job = new SynqClient::DropboxListFilesJob(this);
        job->setToken(m_accessToken);
        auto nam = new QNetworkAccessManager(job);
        job->setNetworkAccessManager(nam);
        job->setUserAgent(Synchronizer::HTTPUserAgent);
        job->setPath("");
        connect(job, &SynqClient::DropboxListFilesJob::finished, this, [=]() {
            auto success = job->error() == SynqClient::JobError::NoError;
            qCDebug(log) << "Account" << uid() << "finished connectivity check with" << success;
            setOnline(success);
            setOnline(success);
            job->deleteLater();
        });
        job->start();
        return;
    }

    // Try to refresh the token:
    qCDebug(log) << "Need to refresh access token for account" << uid();
    auto oauth = createOAuthAuthFlow(this);
    connect(oauth, &QOAuth2AuthorizationCodeFlow::granted, this, [=]() {
        qCDebug(log) << "Token for account" << uid() << "has been refreshed";
        oauth->deleteLater();
        m_refreshToken = oauth->refreshToken();
        m_accessToken = oauth->token();
        m_expiration = oauth->expirationAt();
        setOnline(true);
        emit accountSecretsChanged();
        emit connectivityCheckFinished(true);
    });
    connect(oauth, &QOAuth2AuthorizationCodeFlow::error, this,
            [=](const QString& error, const QString& errorDescription, const QUrl& url) {
                qCWarning(log) << "Failed to refresh tokens for account" << uid();
                qCWarning(log) << "oAuth error:" << error << ":" << errorDescription << url;
                emit connectivityCheckFinished(false);
                oauth->deleteLater();
            });
    oauth->refreshAccessToken();
}
