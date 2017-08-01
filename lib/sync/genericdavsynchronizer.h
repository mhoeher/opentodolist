#ifndef GENERICDAVSYNCHRONIZER_H
#define GENERICDAVSYNCHRONIZER_H

#include <QObject>
#include <QUrl>

#include "sync/webdavsynchronizer.h"


/**
 * @brief A synchronizer for generic WebDAV servers.
 *
 * This synchronizer allows to connect to any WebDAV server. It expects the
 * full WebDAV API URL to be known to the user.
 */
class GenericDAVSynchronizer : public WebDAVSynchronizer
{
    Q_OBJECT
    Q_PROPERTY(QUrl davUrl READ davUrl WRITE setDavUrl NOTIFY davUrlChanged)
public:
    GenericDAVSynchronizer(QObject* parent = nullptr);

    QUrl davUrl() const;
    void setDavUrl(const QUrl& davUrl);

    // Synchronizer interface
    QVariantMap toMap() const override;
    void fromMap(const QVariantMap& map) override;

    // WebDAVSynchronizer interface
    QUrl baseUrl() const override;

signals:

    void davUrlChanged();

private:

    QUrl m_davUrl;
};

#endif // GENERICDAVSYNCHRONIZER_H
