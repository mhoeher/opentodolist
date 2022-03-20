#include "webdavaccount.h"

/**
 * @brief An account used to connect to a WebDAV server.
 *
 * This class implements an account class used to connect to WebDAV servers.
 */

/**
 * @brief Constructor.
 */
WebDAVAccount::WebDAVAccount(QObject* parent)
    : Account { parent },
      m_username(),
      m_password(),
      m_baseUrl(),
      m_disableCertificateChecks(false),
      m_backendSpecificData()
{
    setType(WebDAV);
}

/**
 * @brief The username to use to log in.
 */
QString WebDAVAccount::username() const
{
    return m_username;
}

/**
 * @brief Set the @p username to use to log in.
 */
void WebDAVAccount::setUsername(const QString& username)
{
    if (m_username != username) {
        m_username = username;
        emit usernameChanged();
    }
}

/**
 * @brief The password used to log in to the server.
 */
QString WebDAVAccount::password() const
{
    return m_password;
}

/**
 * @brief Set the password used to log in to the password.
 */
void WebDAVAccount::setPassword(const QString& password)
{
    if (m_password != password) {
        m_password = password;
        emit passwordChanged();
    }
}

/**
 * @brief The base URL used to connect to the server.
 *
 * This is the URL without the protocol part of the server to connect to.
 */
QString WebDAVAccount::baseUrl() const
{
    return m_baseUrl;
}

/**
 * @brief Set the @p baseUrl of the server to connect to.
 */
void WebDAVAccount::setBaseUrl(const QString& baseUrl)
{
    if (m_baseUrl != baseUrl) {
        m_baseUrl = baseUrl;
        emit baseUrlChanged();
    }
}

/**
 * @brief Shall certificate checks be skipped.
 *
 * If this property is false (the default) and secure connections to the
 * server are used, check the HTTPS server certificates. If this property is
 * true, these checks will be skiped. This can be used when connecting to
 * a server which uses self signed certificates.
 */
bool WebDAVAccount::disableCertificateChecks() const
{
    return m_disableCertificateChecks;
}

/**
 * @brief Set if certificate checks shall be skiped.
 */
void WebDAVAccount::setDisableCertificateChecks(bool disableCertificateChecks)
{
    if (m_disableCertificateChecks != disableCertificateChecks) {
        m_disableCertificateChecks = disableCertificateChecks;
        emit disableCertificateChecksChanged();
    }
}

/**
 * @brief Data specific to the used backend.
 */
const QVariantMap& WebDAVAccount::backendSpecificData() const
{
    return m_backendSpecificData;
}

void WebDAVAccount::setBackendSpecificData(const QVariantMap& newBackendSpecificData)
{
    if (m_backendSpecificData == newBackendSpecificData)
        return;
    m_backendSpecificData = newBackendSpecificData;
    emit backendSpecificDataChanged();
}

void WebDAVAccount::save(QSettings* settings)
{
    Account::save(settings);
    settings->setValue("username", m_username);
    settings->setValue("baseUrl", m_baseUrl);
    settings->setValue("disableCertificateChecks", m_disableCertificateChecks);
    settings->setValue("backendSpecificData", m_backendSpecificData);
}

void WebDAVAccount::load(QSettings* settings)
{
    Account::load(settings);
    m_username = settings->value("username", m_username).toString();
    m_baseUrl = settings->value("baseUrl", m_baseUrl).toString();
    m_disableCertificateChecks =
            settings->value("disableCertificateChecks", m_disableCertificateChecks).toBool();
    m_backendSpecificData = settings->value("backendSpecificData", m_backendSpecificData).toMap();
}

QString WebDAVAccount::accountSecrets() const
{
    return m_password;
}

void WebDAVAccount::setAccountSecrets(const QString& secrets)
{
    setPassword(secrets);
}

Synchronizer* WebDAVAccount::createSynchronizer() const
{
    auto sync = new WebDAVSynchronizer;
    sync->setUsername(m_username);
    sync->setPassword(m_password);
    sync->setUrl(m_baseUrl);
    sync->setServerType(WebDAVSynchronizer::Generic);
    return sync;
}
