#include "account.h"


/**
 * @class Account
 * @brief Represents a connection to a server.
 *
 * The account class represents the connection to a server. First of all,
 * it consists of a type, which determines the type of server to talk to.
 * Depending on the concrete type, other properties of the class are relevant,
 * like a server host name, user name and password and so on.
 *
 * Connections to servers are required to synchronize libraries: The app will
 * push data to and pull data from a server in order to ensure user data is
 * backed up and can be accessed also on other devices.
 *
 * Note that one account can be associated with more than one library on
 * a device.
 */

/**
 * @brief Constructor.
 */
Account::Account(QObject *parent) : QObject(parent),
    m_type(Invalid),
    m_username(),
    m_password(),
    m_baseUrl(),
    m_useInsecureConnection(false),
    m_disableCertificateChecks(false)
{

}

/**
 * @brief The type of server to connect to.
 *
 * This property determines the type of server the account represents
 * a connection to.
 */
Account::Type Account::type() const
{
    return m_type;
}


/**
 * @brief Set the account @p type.
 */
void Account::setType(const Type &type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged();
    }
}


/**
 * @brief The username to use to log in.
 */
QString Account::username() const
{
    return m_username;
}


/**
 * @brief Set the @p username to use to log in.
 */
void Account::setUsername(const QString &username)
{
    if (m_username != username) {
        m_username = username;
        emit usernameChanged();
    }
}


/**
 * @brief The password used to log in to the server.
 */
QString Account::password() const
{
    return m_password;
}


/**
 * @brief Set the password used to log in to the password.
 */
void Account::setPassword(const QString &password)
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
QString Account::baseUrl() const
{
    return m_baseUrl;
}


/**
 * @brief Set the @p baseUrl of the server to connect to.
 */
void Account::setBaseUrl(const QString &baseUrl)
{
    if (m_baseUrl != baseUrl) {
        m_baseUrl = baseUrl;
        emit baseUrlChanged();
    }
}


/**
 * @brief Shall an insecure connection to the server be used.
 *
 * This property determines if an insecure connection to the server is used.
 * By default (false), only secure (i.e. `https://`) connection are made.
 * If this property is set to true, insecure (`http://`) ones are used instead.
 */
bool Account::useInsecureConnection() const
{
    return m_useInsecureConnection;
}


/**
 * @brief Set if the account shall use insecure connections to the server.
 */
void Account::setUseInsecureConnection(bool useInsecureConnection)
{
    if (m_useInsecureConnection != useInsecureConnection) {
        m_useInsecureConnection = useInsecureConnection;
        emit useInsecureConnectionChanged();
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
bool Account::disableCertificateChecks() const
{
    return m_disableCertificateChecks;
}


/**
 * @brief Set if certificate checks shall be skiped.
 */
void Account::setDisableCertificateChecks(bool disableCertificateChecks)
{
    if (m_disableCertificateChecks != disableCertificateChecks) {
        m_disableCertificateChecks = disableCertificateChecks;
        emit disableCertificateChecksChanged();
    }
}
