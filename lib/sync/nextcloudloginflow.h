#ifndef NEXTCLOUDLOGINFLOW_H
#define NEXTCLOUDLOGINFLOW_H

#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QSslError>
#include <QTimer>

/**
 * @brief Helper class to get a username and password to log in to NextCloud.
 *
 * This is a helper class which implements NextCloud's Login Flow v2. For more
 * information, please see
 * https://docs.nextcloud.com/server/latest/developer_manual/client_apis/LoginFlow/index.html#login-flow-v2
 *
 * The idea is that the user logs in to the selected NextCloud instance and grants the app
 * access from there. The app will receive a username and password, which also works when
 * e.g. 2 factor authentication (2FA) is enabled.
 */
class NextCloudLoginFlow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool flowRunning READ flowRunning NOTIFY finished)
    Q_PROPERTY(bool ignoreSslErrors READ ignoreSslErrors WRITE setIgnoreSslErrors NOTIFY
                       ignoreSslErrorsChanged)
public:
    explicit NextCloudLoginFlow(QObject* parent = nullptr);

    bool flowRunning() const;

    Q_INVOKABLE bool startLoginFlow(const QUrl& url);

    bool ignoreSslErrors() const;
    void setIgnoreSslErrors(bool ignoreSslErrors);

signals:

    void ignoreSslErrorsChanged();
    void finished();
    void receivedLogin(const QString& username, const QString& password, const QString& server);
    void receivedLoginUrl(const QString& loginUrl);

private:
    QNetworkAccessManager* m_networkAccessManager;
    QTimer* m_pollTimer;
    QString m_token;
    QString m_endpoint;
    bool m_ignoreSslErrors;

    void prepareReply(QNetworkReply* reply);

private slots:

    void onSslErrors(const QList<QSslError>& errors);
    void onFlowKickoffFinished();
    void onPollFinished();
    void poll();
    void parsePollResult(const QByteArray& data);

    void finish();
};

#endif // NEXTCLOUDLOGINFLOW_H
