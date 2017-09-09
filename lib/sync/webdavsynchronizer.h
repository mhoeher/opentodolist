#ifndef WEBDAVSYNCHRONIZER_H
#define WEBDAVSYNCHRONIZER_H

#include <tuple>

#include <QDateTime>
#include <QList>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QSqlDatabase>

#include "sync/synchronizer.h"

class QDir;
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

    enum SyncStepDirection {
        InvalidSyncStep,
        Download,
        Upload
    };

    struct SyncEntry {
        QString parent;
        QString entry;
        EntryType localType;
        EntryType remoteType;
        QDateTime lastModDate;
        QDateTime previousLasModtDate;
        QString etag;
        QString previousEtag;

        SyncEntry() :
            parent(),
            entry(),
            localType(Invalid),
            remoteType(Invalid),
            lastModDate(),
            previousLasModtDate(),
            etag(),
            previousEtag()
        {
        }

        QString path() const {
            if (parent.isEmpty() && entry.isEmpty()) {
                return "";
            } else if (parent.isEmpty()) {
                return entry;
            } else {
                return parent + "/" + entry;
            }
        }
    };

    typedef QList<Entry> EntryList;
    typedef QMap<QString, SyncEntry> SyncEntryMap;


    QNetworkAccessManager *m_networkAccessManager;
    QString m_remoteDirectory;
    bool m_disableCertificateCheck;
    QString m_username;
    QString m_password;


    EntryList entryList(const QString& directory, bool* ok = nullptr);
    bool download(const QString& filename);
    bool upload(const QString& filename, QString *etag = nullptr);
    bool mkdir(const QString& dirname, QString *etag = nullptr);
    bool deleteEntry(const QString& filename);
    bool syncDirectory(const QString &directory,
            QRegularExpression directoryFilter = QRegularExpression(".*"),
            bool pushOnly = false, QSet<QString> *changedDirs = nullptr);
    QString etag(const QString &filename);

    // Path and URL utility functions
    static QString mkpath(const QString &path);
    static std::tuple<QString, QString> splitpath(const QString& path);
    QString urlString() const;


    QNetworkReply *listDirectoryRequest(const QString& directory);
    QNetworkReply *etagRequest(const QString& filename);
    QNetworkReply *createDirectoryRequest(const QString& directory);
    EntryList parseEntryList(const QString& directory,
                             const QByteArray& reply);
    EntryList parsePropFindResponse(const QDomDocument& response,
                                    const QString& directory);
    Entry parseResponseEntry(const QDomElement& element,
                             const QString& baseDir);
    void prepareReply(QNetworkReply* reply) const;
    void waitForReplyToFinish(QNetworkReply* reply) const;

    // Sync DB Handling
    QSqlDatabase openSyncDb();
    void closeSyncDb();
    void insertSyncDBEntry(QSqlDatabase &db, const SyncEntry &entry);
    SyncEntryMap findSyncDBEntries(QSqlDatabase &db,
                                              const QString& parent);
    void removeDirFromSyncDB(QSqlDatabase &db, const SyncEntry &entry);
    void removeFileFromSyncDB(QSqlDatabase &db, const SyncEntry &entry);

    // File System Utils
    bool rmLocalDir(const QString& dir, int maxDepth = 0);

    // syncDirectory() split down methods:
    void mergeLocalInfoWithSyncList(
            QDir &d, const QString &dir, SyncEntryMap &entries);
    bool mergeRemoteInfoWithSyncList(SyncEntryMap &entries, const QString &dir);
    bool pullEntry(SyncEntry& entry, QSqlDatabase& db);
    bool removeLocalEntry(SyncEntry& entry, QSqlDatabase& db);
    bool pushEntry(SyncEntry& entry, QSqlDatabase& db);
    bool removeRemoteEntry(SyncEntry& entry, QSqlDatabase& db);
    bool skipEntry(const SyncEntry &entry, SyncStepDirection direction, const QRegularExpression &dirFilter);
};


Q_DECLARE_LOGGING_CATEGORY(webDAVSynchronizer)

#endif // WEBDAVSYNCHRONIZER_H
