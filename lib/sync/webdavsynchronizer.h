#ifndef WEBDAVSYNCHRONIZER_H
#define WEBDAVSYNCHRONIZER_H

#include <QList>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QtConcurrent>

#include "sync/synchronizer.h"


class WebDAVClient;


class WebDAVSynchronizer : public Synchronizer
{
    Q_OBJECT
    Q_PROPERTY(QString remoteDirectory READ remoteDirectory WRITE setRemoteDirectory NOTIFY remoteDirectoryChanged)
    Q_PROPERTY(bool disableCertificateCheck READ disableCertificateCheck WRITE setDisableCertificateCheck NOTIFY disableCertificateCheckChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(WebDAVServerType serverType READ serverType WRITE setServerType NOTIFY serverTypeChanged)

#ifdef WEBDAV_SYNCHRONIZER_TEST
    friend class WebDAVSynchronizerTest;
#endif

public:

    enum WebDAVServerType {
        Unknown = 0,
        Generic = 1,
        NextCloud = 2,
        OwnCloud = 3
    };

    Q_ENUM(WebDAVServerType)

    explicit WebDAVSynchronizer(QObject* parent = nullptr);
    virtual ~WebDAVSynchronizer();

    QUrl baseUrl() const;

    // Synchronizer interface
    void validate() override;
    void synchronize() override;
    void findExistingLibraries() override;
    QVariantMap toMap() const override;
    void fromMap(const QVariantMap& map) override;
    QString secretsKey() const override;
    QString secret() const override;
    void setSecret(const QString &secret) override;

    QString remoteDirectory() const;
    void setRemoteDirectory(const QString& remoteDirectory);

    bool disableCertificateCheck() const;
    void setDisableCertificateCheck(bool disableCertificateCheck);

    QString username() const;
    void setUsername(const QString& username);

    QString password() const;
    void setPassword(const QString& password);

    QUrl url() const;
    void setUrl(const QUrl &url);

    WebDAVClient *createDAVClient(QObject* parent = nullptr);

    WebDAVServerType serverType() const;
    void setServerType(const WebDAVServerType &serverType);

    bool createDirs() const;
    void setCreateDirs(bool createDirs);

signals:
    void remoteDirectoryChanged();
    void disableCertificateCheckChanged();
    void usernameChanged();
    void passwordChanged();
    void urlChanged();
    void serverTypeChanged();

private:

    QUrl    m_url;
    QString m_remoteDirectory;
    bool m_disableCertificateCheck;
    QString m_username;
    QString m_password;
    bool m_createDirs;
    WebDAVServerType m_serverType;
    QFutureWatcher<QVariantList> m_findExistingEntriesWatcher;
};


Q_DECLARE_LOGGING_CATEGORY(webDAVSynchronizer)

#endif // WEBDAVSYNCHRONIZER_H
