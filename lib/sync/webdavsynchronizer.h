#ifndef WEBDAVSYNCHRONIZER_H
#define WEBDAVSYNCHRONIZER_H

#include <QList>
#include <QLoggingCategory>

#include "sync/synchronizer.h"

class QDomDocument;
class QDomElement;
class QNetworkAccessManager;
class QNetworkReply;


#ifdef WEBDAV_SYNCHRONIZER_TEST
class WebDAVSynchronizerTest;
#endif

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

signals:

    void remoteDirectoryChanged();
    void disableCertificateCheckChanged();
    void usernameChanged();
    void passwordChanged();

private:

    enum EntryType {
        Invalid,
        File,
        Directory
    };

    struct Entry {
        QString name;
        EntryType type;
        QString etag;
    };

    typedef QList<Entry> EntryList;


    QNetworkAccessManager *m_networkAccessManager;
    QString m_remoteDirectory;
    bool m_disableCertificateCheck;
    QString m_username;
    QString m_password;


    EntryList entryList(const QString& directory, bool* ok = nullptr);
    bool download(const QString& filename);
    bool upload(const QString& filename);
    bool mkdir(const QString& dirname);
    bool deleteEntry(const QString& filename);

    QNetworkReply *listDirectoryRequest(const QString& directory);
    QNetworkReply *createDirectoryRequest(const QString& directory);
    EntryList parseEntryList(const QString& directory,
                             const QByteArray& reply);
    EntryList parsePropFindResponse(const QDomDocument& response,
                                    const QString& directory);
    Entry parseResponseEntry(const QDomElement& element,
                             const QString& baseDir);
    void prepareReply(QNetworkReply* reply) const;
    void waitForReplyToFinish(QNetworkReply* reply) const;

};


Q_DECLARE_LOGGING_CATEGORY(webDAVSynchronizer)

#endif // WEBDAVSYNCHRONIZER_H
