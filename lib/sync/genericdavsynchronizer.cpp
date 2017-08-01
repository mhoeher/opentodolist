#include "genericdavsynchronizer.h"


/**
 * @brief Constructor.
 */
GenericDAVSynchronizer::GenericDAVSynchronizer(QObject* parent) :
    WebDAVSynchronizer(parent),
    m_davUrl()
{
}


/**
 * @brief The WebDAV API URL to be used for talking to the server.
 *
 * @sa setDavUrl()
 */
QUrl GenericDAVSynchronizer::davUrl() const
{
    return m_davUrl;
}


/**
 * @brief Set the WebDAV API URL to be used for talking to the server.
 *
 * @sa davUrl()
 */
void GenericDAVSynchronizer::setDavUrl(const QUrl& davUrl)
{
    if (m_davUrl != davUrl) {
        m_davUrl = davUrl;
        emit davUrlChanged();
    }
}


QVariantMap GenericDAVSynchronizer::toMap() const
{
    auto result = WebDAVSynchronizer::toMap();
    result["davUrl"] = m_davUrl;
    return result;
}

void GenericDAVSynchronizer::fromMap(const QVariantMap& map)
{
    m_davUrl = map.value("davUrl").toUrl();
    WebDAVSynchronizer::fromMap(map);
}


/**
 * @brief The base URL where the WebDAV server is listening on.
 *
 * This is identical to the davUrl property, i.e. if this class is used for
 * synchronization, the full WebDAV URL must be known by the
 * user. If available, it is better to use one of the dedicated
 * classes, as they automatically generate the full URL required.
 */
QUrl GenericDAVSynchronizer::baseUrl() const
{
    return m_davUrl;
}
