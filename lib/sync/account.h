#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>
#include <QUuid>

class QSettings;

class Account : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Account::Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername
               NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword
               NOTIFY passwordChanged)
    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl
               NOTIFY baseUrlChanged)
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


    QUuid uid() const;
    void setUid(const QUuid &uid);

    Type type() const;
    void setType(const Type &type);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

    QString baseUrl() const;
    void setBaseUrl(const QString &baseUrl);

    bool disableCertificateChecks() const;
    void setDisableCertificateChecks(bool disableCertificateChecks);

    void save(QSettings *settings);
    void load(QSettings *settings);

    QString name() const;
    void setName(const QString &name);

signals:

    void uidChanged();
    void typeChanged();
    void nameChanged();
    void usernameChanged();
    void passwordChanged();
    void baseUrlChanged();
    void disableCertificateChecksChanged();

private:

    QUuid m_uid;
    Type m_type;
    QString m_name;
    QString m_username;
    QString m_password;
    QString m_baseUrl;
    bool m_disableCertificateChecks;

};

#endif // ACCOUNT_H
