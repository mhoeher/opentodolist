#ifndef WEBDAVACCOUNT_H
#define WEBDAVACCOUNT_H

#include <QObject>
#include <QString>
#include <QVariantMap>

#include "account.h"

class WebDAVAccount : public Account
{
    Q_OBJECT
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(bool disableCertificateChecks READ disableCertificateChecks WRITE
                       setDisableCertificateChecks NOTIFY disableCertificateChecksChanged)
    Q_PROPERTY(QVariantMap backendSpecificData READ backendSpecificData WRITE setBackendSpecificData
                       NOTIFY backendSpecificDataChanged)
public:
    explicit WebDAVAccount(QObject* parent = nullptr);

    QString username() const;
    void setUsername(const QString& username);

    QString password() const;
    void setPassword(const QString& password);

    QString baseUrl() const;
    void setBaseUrl(const QString& baseUrl);

    bool disableCertificateChecks() const;
    void setDisableCertificateChecks(bool disableCertificateChecks);

    const QVariantMap& backendSpecificData() const;
    void setBackendSpecificData(const QVariantMap& newBackendSpecificData);

signals:

    void usernameChanged();
    void passwordChanged();
    void baseUrlChanged();
    void disableCertificateChecksChanged();

    void backendSpecificDataChanged();

private:
    QString m_username;
    QString m_password;
    QString m_baseUrl;
    bool m_disableCertificateChecks;
    QVariantMap m_backendSpecificData;

    // Account interface
public:
    void save(QSettings* settings) override;
    void load(QSettings* settings) override;
    QString accountSecrets() const override;
    void setAccountSecrets(const QString& secrets) override;
    Synchronizer* createSynchronizer() const override;
};

#endif // WEBDAVACCOUNT_H
