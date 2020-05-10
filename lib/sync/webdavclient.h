/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SYNC_WEBDAVCLIENT_H_
#define SYNC_WEBDAVCLIENT_H_

#include <QDateTime>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QObject>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QUrl>

#include <tuple>

class QDir;
class QDomDocument;
class QDomElement;
class QNetworkAccessManager;
class QNetworkReply;

class WebDAVSynchronizer;

#ifdef WEBDAV_SYNCHRONIZER_TEST
class WebDAVSynchronizerTest;
#endif

class WebDAVClient : public QObject
{
    Q_OBJECT
    friend class WebDAVSynchronizer;
#ifdef WEBDAV_SYNCHRONIZER_TEST
    friend class WebDAVSynchronizerTest;
#endif
public:
    explicit WebDAVClient(QObject *parent = nullptr);

    QUrl baseUrl() const;
    void setBaseUrl(const QUrl &baseUrl);

    QString remoteDirectory() const;
    void setRemoteDirectory(const QString &remoteDirectory);

    QString directory() const;
    void setDirectory(const QString &directory);

    bool disableCertificateCheck() const;
    void setDisableCertificateCheck(bool disableCertificateCheck);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

signals:

    void stopRequested();

    void debug(const QString &message) const;
    void warning(const QString &message) const;
    void error(const QString &message) const;

    /**
     * @brief Indicates errors during synchronization.
     *
     * This signal is emitted to indicate issues during sync operations.
     * The @p message string contains more detailed information.
     */
    void syncError(const QString &message) const;

public slots:

    void stopSync();

private:
    enum EntryType { Invalid, File, Directory };

    struct Entry
    {
        QString name;
        EntryType type;
        QString etag;
    };

    enum SyncStepDirection { InvalidSyncStep, Download, Upload };

    struct SyncEntry
    {
        QString parent;
        QString entry;
        EntryType localType;
        EntryType remoteType;
        QDateTime lastModDate;
        QDateTime previousLasModDate;
        QString etag;
        QString previousEtag;

        SyncEntry()
            : parent(),
              entry(),
              localType(Invalid),
              remoteType(Invalid),
              lastModDate(),
              previousLasModDate(),
              etag(),
              previousEtag()
        {
        }

        QString path() const
        {
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
    QUrl m_baseUrl;
    QString m_remoteDirectory;
    QString m_directory;
    bool m_disableCertificateCheck;
    QString m_username;
    QString m_password;
    bool m_stopRequested;

    EntryList entryList(const QString &directory, bool *ok = nullptr);
    bool download(const QString &filename, QIODevice *targetDevice = nullptr);
    QByteArray getRemoteFileContents(const QString &filename);
    bool upload(const QString &filename, QString *etag = nullptr);
    bool mkdir(const QString &dirname, QString *etag = nullptr);
    bool deleteEntry(const QString &filename);
    bool syncDirectory(const QString &directory,
                       QRegularExpression directoryFilter = QRegularExpression(".*"),
                       bool pushOnly = false, QSet<QString> *changedDirs = nullptr);
    QString etag(const QString &filename);

    // Path and URL utility functions
    static QString mkpath(const QString &path);
    static std::tuple<QString, QString> splitpath(const QString &path);
    QString urlString() const;

    QNetworkReply *listDirectoryRequest(const QString &directory);
    QNetworkReply *etagRequest(const QString &filename);
    QNetworkReply *createDirectoryRequest(const QString &directory);
    static EntryList parseEntryList(const QUrl &baseUrl, const QString &directory,
                                    const QByteArray &reply);
    static EntryList parsePropFindResponse(const QUrl &baseUrl, const QDomDocument &response,
                                           const QString &directory);
    static Entry parseResponseEntry(const QDomElement &element, const QString &baseDir);
    void prepareReply(QNetworkReply *reply) const;
    static void waitForReplyToFinish(QNetworkReply *reply);

    // Sync DB Handling
    QSqlDatabase openSyncDb();
    void closeSyncDb();
    void insertSyncDBEntry(QSqlDatabase *db, const SyncEntry &entry);
    SyncEntryMap findSyncDBEntries(QSqlDatabase *db, const QString &parent);
    void removeDirFromSyncDB(QSqlDatabase *db, const SyncEntry &entry);
    void removeFileFromSyncDB(QSqlDatabase *db, const SyncEntry &entry);

    // File System Utils
    bool rmLocalDir(const QString &dir, int maxDepth = 0);

    // syncDirectory() split down methods:
    void mergeLocalInfoWithSyncList(const QDir &d, const QString &dir, SyncEntryMap &entries);
    bool mergeRemoteInfoWithSyncList(SyncEntryMap &entries, const QString &dir);
    bool pullEntry(SyncEntry &entry, QSqlDatabase *db);
    bool removeLocalEntry(SyncEntry &entry, QSqlDatabase *db);
    bool pushEntry(SyncEntry &entry, QSqlDatabase *db);
    bool removeRemoteEntry(const SyncEntry &entry, QSqlDatabase *db);
    bool skipEntry(const SyncEntry &entry, SyncStepDirection direction,
                   const QRegularExpression &dirFilter);
};

enum class HTTPStatusCode {
    Continue = 100,
    SwitchingProtocols = 101,
    WebDAVProcessing = 102,
    OK = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,
    WebDAVMultiStatus = 207,
    WebDAVAlreadyReported = 208,
    IMUsed = 226,
    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    UseProxy = 305,
    Unused = 306,
    TemporaryRedirect = 307,
    PermanentRedirect = 308,
    BadRequest = 400,
    Unauthorized = 401,
    PaymentRequired = 402,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    ProxyAuthenticationRequired = 407,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    LengthRequired = 411,
    PreconditionFailed = 412,
    RequestEntityTooLarge = 413,
    RequestURITooLong = 414,
    UnsupportedMediaType = 415,
    RequestedRangeNotSatisfiable = 416,
    ExpectationFailed = 417,
    ImATeapot = 418,
    TwiterEnhanceYourCalm = 420,
    WebDAVUnprocessableEntity = 422,
    WebDAVLocked = 423,
    WebDAVFailedDependency = 424,
    WebDAVReserved = 425,
    UpgradeRequired = 426,
    PreconditionRequired = 428,
    TooManyRequests = 429,
    RequestHeaderFieldsTooLarge = 431,
    NginxNoResponse = 444,
    MicrosoftRetryWith = 449,
    MicrosoftBlockedByWindowsParentalControls = 450,
    UnavailableForLegalReasons = 451,
    NginxClientClosedRequest = 499,
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HTTPVersionNotSupported = 505,
    VariantAlsoNegotiates = 506,
    WebDAVInsufficientStorage = 507,
    WebDAVLoopDetected = 508,
    ApacheBandwidthLimitExceeded = 509,
    NotExtended = 510,
    NetworkAuthenticationRequired = 511,
    NetworkReadTimeoutError = 598,
    NetworkConnectTimeoutError = 599
};

inline bool operator==(int lhs, HTTPStatusCode rhs)
{
    return lhs == static_cast<int>(rhs);
}

inline bool operator==(HTTPStatusCode lhs, int rhs)
{
    return static_cast<int>(lhs) == rhs;
}

#endif // SYNC_WEBDAVCLIENT_H_
