#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>

class Account : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Account::Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername
               NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword
               NOTIFY passwordChanged)
    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl
               NOTIFY baseUrlChanged)
    Q_PROPERTY(bool useInsecureConnection READ useInsecureConnection
               WRITE setUseInsecureConnection
               NOTIFY useInsecureConnectionChanged)
    Q_PROPERTY(bool disableCertificateChecks READ disableCertificateChecks
               WRITE setDisableCertificateChecks
               NOTIFY disableCertificateChecksChanged)
public:
    explicit Account(QObject *parent = nullptr);

    enum Type {
        Invalid = 0,
        WebDAV,
        NextCloud,
        OwnCloud
    };

    Q_ENUM(Type);


    Type type() const;
    void setType(const Type &type);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

    QString baseUrl() const;
    void setBaseUrl(const QString &baseUrl);

    bool useInsecureConnection() const;
    void setUseInsecureConnection(bool useInsecureConnection);

    bool disableCertificateChecks() const;
    void setDisableCertificateChecks(bool disableCertificateChecks);

signals:

    void typeChanged();
    void usernameChanged();
    void passwordChanged();
    void baseUrlChanged();
    void useInsecureConnectionChanged();
    void disableCertificateChecksChanged();

private:

    Type m_type;
    QString m_username;
    QString m_password;
    QString m_baseUrl;
    bool m_useInsecureConnection;
    bool m_disableCertificateChecks;

};

#endif // ACCOUNT_H
