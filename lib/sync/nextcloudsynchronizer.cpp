#include "nextcloudsynchronizer.h"


const QString NextCloudSynchronizer::WebDAVAPIPath = "/remote.php/webdav/";

/**
 * @brief Constructor.
 */
NextCloudSynchronizer::NextCloudSynchronizer(QObject* parent) :
    WebDAVSynchronizer(parent),
    m_url()
{
}


/**
 * @brief The base URL of the NextCloud instance to connect to.
 *
 * @sa setUrl()
 */
QUrl NextCloudSynchronizer::url() const
{
    return m_url;
}


/**
 * @brief Set the base URL of the NextCloud instance to sync with.
 *
 * @sa url()
 */
void NextCloudSynchronizer::setUrl(const QUrl& url)
{
    if (m_url != url) {
        m_url = url;
        emit urlChanged();
    }
}


QVariantMap NextCloudSynchronizer::toMap() const
{
    auto result = WebDAVSynchronizer::toMap();
    result["url"] = m_url;
    return result;
}


void NextCloudSynchronizer::fromMap(const QVariantMap& map)
{
    m_url = map.value("url").toUrl();
    WebDAVSynchronizer::fromMap(map);
}


/**
 * @brief The WebDAV base URL.
 *
 * This returns the WebDAV base URL required for synchronization. It
 * is derived from the NextCloud instance URL plus a specific path
 * added for the DAV API.
 */
QUrl NextCloudSynchronizer::baseUrl() const
{
    QUrl result;
    if (m_url.isValid()) {
       result = QUrl(m_url.toString() + WebDAVAPIPath);
    }
    return result;
}
