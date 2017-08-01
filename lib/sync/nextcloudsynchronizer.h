#ifndef NEXTCLOUDSYNCHRONIZER_H
#define NEXTCLOUDSYNCHRONIZER_H

#include <QUrl>
#include <QObject>

#include "sync/webdavsynchronizer.h"


/**
 * @brief Synchronize data with NextCloud or ownCloud.
 *
 * This synchronizer allows to sync libraries on a NextCloud or ownCloud
 * instance. It basically is a simple extension to the generic
 * WebDAVSynchronizer, automatically adding the correct WebDAV base URL
 * given the NextCloud/ownCloud instance URL.
 */
class NextCloudSynchronizer : public WebDAVSynchronizer
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
public:

    static const QString WebDAVAPIPath;

    NextCloudSynchronizer(QObject* parent = nullptr);

    QUrl url() const;
    void setUrl(const QUrl& url);

    // Synchronizer interface
    QVariantMap toMap() const override;
    void fromMap(const QVariantMap& map) override;

    // WebDAVSynchronizer interface
    QUrl baseUrl() const override;

signals:

    void urlChanged();

private:

    QUrl m_url;
};

#endif // NEXTCLOUDSYNCHRONIZER_H
