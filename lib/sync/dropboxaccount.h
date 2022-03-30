#ifndef DROPBOXACCOUNT_H
#define DROPBOXACCOUNT_H

#include <QObject>

#include "account.h"

class DropboxAccount : public Account
{
    Q_OBJECT

    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)

public:
    explicit DropboxAccount(QObject* parent = nullptr);

    const QString& accessToken() const;
signals:

    void accessTokenChanged();

private:
    // OAuth props
    QString m_refreshToken;
    QString m_accessToken;
    QDateTime m_expiration;

    // PKCE extension:
    QString m_codeVerifier;

    void setAccessToken(const QString& newAccessToken);

    // Account interface
public:
    void save(QSettings* settings) override;
    void load(QSettings* settings) override;
    QString accountSecrets() const override;
    void setAccountSecrets(const QString& secrets) override;
    Synchronizer* createSynchronizer() const override;
};

#endif // DROPBOXACCOUNT_H
