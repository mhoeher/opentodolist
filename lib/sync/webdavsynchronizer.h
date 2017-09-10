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

#ifdef WEBDAV_SYNCHRONIZER_TEST
    friend class WebDAVSynchronizerTest;
#endif

public:

    explicit WebDAVSynchronizer(QObject* parent = nullptr);
    virtual ~WebDAVSynchronizer();

    virtual QUrl baseUrl() const = 0;

    // Synchronizer interface
    void validate() override;
    void synchronize() override;
    void findExistingLibraries() override;
    QVariantMap toMap() const override;
    void fromMap(const QVariantMap& map) override;

    QString remoteDirectory() const;
    void setRemoteDirectory(const QString& remoteDirectory);

    bool disableCertificateCheck() const;
    void setDisableCertificateCheck(bool disableCertificateCheck);

    QString username() const;
    void setUsername(const QString& username);

    QString password() const;
    void setPassword(const QString& password);

    WebDAVClient *createDAVClient(QObject* parent = nullptr);

signals:

    void remoteDirectoryChanged();
    void disableCertificateCheckChanged();
    void usernameChanged();
    void passwordChanged();

private:

    QString m_remoteDirectory;
    bool m_disableCertificateCheck;
    QString m_username;
    QString m_password;
    QFutureWatcher<QVariantList> m_findExistingEntriesWatcher;
};


Q_DECLARE_LOGGING_CATEGORY(webDAVSynchronizer)

#endif // WEBDAVSYNCHRONIZER_H
