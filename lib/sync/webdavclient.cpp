#include "webdavclient.h"

#include <QCoreApplication>
#include <QBuffer>
#include <QDateTime>
#include <QDir>
#include <QDomDocument>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTemporaryFile>
#include <QTimer>


static Q_LOGGING_CATEGORY(log, "OpenTodoList.WebDAVClient", QtDebugMsg)

WebDAVClient::WebDAVClient(QObject *parent) : QObject(parent),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_baseUrl(),
    m_remoteDirectory(),
    m_directory(),
    m_disableCertificateCheck(false),
    m_username(),
    m_password(),
    m_stopRequested(false)
{
}

QString WebDAVClient::remoteDirectory() const
{
    return m_remoteDirectory;
}

void WebDAVClient::setRemoteDirectory(const QString &remoteDirectory)
{
    m_remoteDirectory = remoteDirectory;
}

QString WebDAVClient::directory() const
{
    return m_directory;
}

void WebDAVClient::setDirectory(const QString &directory)
{
    m_directory = directory;
}

bool WebDAVClient::disableCertificateCheck() const
{
    return m_disableCertificateCheck;
}

void WebDAVClient::setDisableCertificateCheck(bool disableCertificateCheck)
{
    m_disableCertificateCheck = disableCertificateCheck;
}

QString WebDAVClient::username() const
{
    return m_username;
}

void WebDAVClient::setUsername(const QString &username)
{
    m_username = username;
}

QString WebDAVClient::password() const
{
    return m_password;
}

void WebDAVClient::setPassword(const QString &password)
{
    m_password = password;
}

void WebDAVClient::stopSync()
{
    m_stopRequested = true;
    emit stopRequested();
}

QUrl WebDAVClient::baseUrl() const
{
    return m_baseUrl;
}

void WebDAVClient::setBaseUrl(const QUrl &baseUrl)
{
    m_baseUrl = baseUrl;
}

/**
 * @brief Get the list of entries on the given @p directory.
 *
 * This retrieves the contents of the @p directory (which is relative to the
 * remoteDirectory set in the WebDAVClient itself).
 */
WebDAVClient::EntryList WebDAVClient::entryList(const QString& directory, bool *ok)
{
    EntryList result;
    auto dir = this->remoteDirectory() + "/" + directory;
    dir = QDir::cleanPath(dir);
    auto reply = listDirectoryRequest(dir);
    connect(reply, &QNetworkReply::finished,
            reply, &QNetworkReply::deleteLater);
    bool status = false;
    waitForReplyToFinish(reply);
    auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (code == HTTPStatusCode::WebDAVMultiStatus) {
        result = parseEntryList(baseUrl(), dir, reply->readAll());
        status = true;
    } else {
        emit warning(tr("Unexpected HTTP code received when getting "
                        "remote folder entry list: '%1'").arg(code));
    }
    if (ok != nullptr) {
        *ok = status;
    }
    return result;
}


/**
 * @brief Download a file.
 *
 * This downloads the file named @p filename which is relative to the currently
 * set remoteDirectory(). By default, the file will be stored in a file
 * names @p filename relative to the currently set directory(). If a
 * @p targetDevice is set, the downloaded content is written to that device
 * instead.
 */
bool WebDAVClient::download(const QString& filename, QIODevice *targetDevice)
{
    QTemporaryFile* file = new QTemporaryFile();
    auto result = false;
    if (file->open()) {
        QNetworkRequest request;
        auto url = QUrl(urlString() +
                        mkpath(remoteDirectory() + "/" + filename));
        url.setUserName(username());
        url.setPassword(password());
        request.setUrl(url);
        auto reply = m_networkAccessManager->get(request);
        file->setParent(reply);
        connect(reply, &QNetworkReply::finished,
                reply, &QNetworkReply::deleteLater);
        connect(reply, &QNetworkReply::readyRead, [=]() {
            file->write(reply->readAll());
        });
        connect(qApp, &QCoreApplication::aboutToQuit,
                reply, &QNetworkReply::abort);
        waitForReplyToFinish(reply);
        file->write(reply->readAll());
        auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (code == HTTPStatusCode::OK) {
            if (targetDevice != nullptr) {
                file->seek(0);
                while (!file->atEnd()) {
                    targetDevice->write(file->read(1024*1024));
                }
                result = true;
            } else {
                QFile targetFile(directory() + "/" + filename);
                if (targetFile.open(QIODevice::WriteOnly)) {
                    file->seek(0);
                    while (!file->atEnd()) {
                        targetFile.write(file->read(1024*1024));
                    }
                    targetFile.close();
                    result = true;
                } else {
                    qCWarning(log) << "Failed to open destination"
                                                  << "file for writing:"
                                                  << targetFile.errorString();
                    emit warning(tr("Failed to open file '%1' for writing: %2")
                                 .arg(targetFile.fileName())
                                 .arg(targetFile.errorString()));
                }
            }
        } else {
            qCWarning(log) << "Download failed with code" << code;
            emit warning(tr("Download failed with HTTP code %1").arg(code));
        }
    } else {
        qCWarning(log) << "Failed to open temporary file for"
                                      << "downloading:"
                                      << file->errorString();
        emit warning(tr("Failed to open intermediate download file: %1")
                     .arg(file->errorString()));
    }
    return result;
}


/**
 * @brief Upload a local file to the server.
 *
 * This method uploads the file speficied by the @p filename which is relative
 * to the set local directory name to the server (relative to the set remote
 * directory name). If @ etag points to a string, the new etag of the file
 * is stored in it.
 *
 * Note: WebDAV servers might not report etags when uploading files. In this
 * case if @p etag is not a null pointer, a separate PROPFIND request is done
 * to get the etag of the file just uploaded. However, this request might
 * retrieve wrong results (e.g. if another client uploaded the same file
 * in the meantime and hence overwrote it).
 */
bool WebDAVClient::upload(const QString& filename, QString* etag)
{
    auto result = false;
    auto file = new QFile(directory() + "/" + filename);
    QString currentEtag = "no-etag-retrieved-yet";
    if (file->open(QIODevice::ReadOnly)) {
        QNetworkRequest request;
        auto url = QUrl(urlString() +
                        mkpath(remoteDirectory() + "/" + filename));
        url.setUserName(username());
        url.setPassword(password());
        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentLengthHeader,
                          file->size());
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          "application/octet-stream");
        auto reply = m_networkAccessManager->put(request, file);
        connect(qApp, &QCoreApplication::aboutToQuit,
                reply, &QNetworkReply::abort);
        connect(reply, &QNetworkReply::finished,
                reply, &QNetworkReply::deleteLater);
        file->setParent(reply);
        waitForReplyToFinish(reply);

        auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (code == HTTPStatusCode::OK || code == HTTPStatusCode::Created ||
                code == HTTPStatusCode::NoContent) {
            for (auto header : reply->rawHeaderPairs()) {
                if (header.first.toLower() == "etag") {
                    currentEtag = header.second;
                }
            }
            result = true;
        } else {
            qCWarning(log) << "Upload failed with code" << code;
            emit warning(tr("Uploading failed with HTTP code %1").arg(code));
        }
    } else {
        qCWarning(log) << "Failed to open" << filename
                                      << "for reading:" << file->errorString();
        emit warning(tr("Failed to open file '%1' for reading: %2")
                     .arg(file->fileName()).arg(file->errorString()));
        delete file;
    }

    if (etag != nullptr) {
        if (currentEtag.isNull()) {
            qDebug() << "Server did not send etag on upload - "
                        "manually getting it.";
            emit debug(tr("Server did not send eTag when uploading"));
            currentEtag = this->etag(filename);
        }
        *etag = currentEtag;
    }
    return result;
}

bool WebDAVClient::mkdir(const QString& dirname, QString *etag)
{
    auto result = false;
    auto reply = createDirectoryRequest(this->remoteDirectory() + "/" + dirname);
    connect(reply, &QNetworkReply::finished,
            reply, &QNetworkReply::deleteLater);
    QString currentEtag = "no-etag-retrieved-yet";
    waitForReplyToFinish(reply);
    auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    result = code.toInt() == HTTPStatusCode::Created;
    for (auto header : reply->rawHeaderPairs()) {
        if (header.first.toLower() == "etag") {
            currentEtag = header.second;
        }
    }
    if (etag != nullptr) {
        *etag = currentEtag;
    }
    return result;
}

bool WebDAVClient::deleteEntry(const QString& filename)
{
    auto result = false;
    QNetworkRequest request;
    auto url = QUrl(urlString() +
                    mkpath(remoteDirectory() + "/" + filename));
    url.setUserName(username());
    url.setPassword(password());
    request.setUrl(url);
    auto reply = m_networkAccessManager->deleteResource(request);
    connect(reply, &QNetworkReply::finished,
            reply, &QNetworkReply::deleteLater);
    connect(qApp, &QCoreApplication::aboutToQuit,
            reply, &QNetworkReply::abort);
    waitForReplyToFinish(reply);
    auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (code == HTTPStatusCode::OK || code == HTTPStatusCode::NoContent) {
        result = true;
    } else {
        qCWarning(log) << "Deleting entry failed with code"
                                      << code;
        emit warning(tr("Deleting remote file failed with HTTP code %1")
                     .arg(code));
    }
    return result;
}


/**
 * @brief Synchronize a single directory.
 *
 * This method synchronizes a local directory with a remote one. The
 * @p directory is relative to the local and remote one set in the
 * synchronizer. The synchronization works in both directions, i.e. local
 * changes are pushed to the server and remote changes are pulled down.
 * The procedure is implemented with a server-first approach, i.e. if there
 * are two conflicting changes the change from the server has precendence
 * over a local change. This method only syncs files which do not
 * start with a dot character (i.e. UNIX hidden files). Sub-directories are
 * synchronized non-recursively, i.e. only the directory itself is
 * synchronized but not its contents. The @p directoryFilter can be used to
 * limit synchronization to only those sub-directories matching the given
 * regular expression. By default, all directories are synchronized.
 *
 * The synchronization procedure works the following way:
 * A list of objects is created, which hold the local last modification
 * date (MD), the local last modification date from the previous
 * sync (PMD), the current etag (ETAG) and the etag from the previous
 * snyc run (PETAG). The previous (P...) values are stored in a local
 * datbase inside the local directory. This database itself is not
 * synchronized.
 *
 * MD might be null if the file does not exist locally.
 * PMD might be null if the file did not exist locally during the last
 * sync. ETAG might be null if the file does not exist on the server. PETAG
 * might be null if the file did not exist on the server during the last sync.
 *
 * The following rules are applied in order:
 *
 * - If ETAG is not null and ETAG is unequal to PETAG, then download the file.
 * - If ETAG is null and PETAG is not nnull, delete the local file.
 * - If MD is not null and PMD is null or MD is unqual to PMD,
 *   upload the file.
 * - If MD is null and PMD is not null, delete the remote file.
 *
 * If @p pushOnly is true, the method will not attempt to get the remote
 * list of directory contents. This can be used if a higher level method
 * detected that the directory is unchanged. In this case, only the local
 * files are checked and uploaded.
 *
 * If @p changedDirs is not a null pointer, any directory for which a change
 * is detected is added to the set. This set can then be used to optimize
 * recursive syncs in higher level functions, i.e. if a directory is not
 * in the set, no changes compared to the last sync run were detected
 * and hence only local changes need to be pushed.
 */
bool WebDAVClient::syncDirectory(
        const QString& directory,
        QRegularExpression directoryFilter,
        bool pushOnly,
        QSet<QString> *changedDirs)
{
    m_stopRequested = false;
    QSet<QString> _changedDirs;
    bool result = false;
    auto localBase = this->directory();
    auto dir = mkpath(directory);
    QDir d(localBase + "/" + dir);
    QString dbConnName;
    if (!localBase.isEmpty() && d.exists()) {
        result = true;

        auto db = openSyncDb();
        dbConnName = db.databaseName();
        auto entries = findSyncDBEntries(db, dir);

        mergeLocalInfoWithSyncList(d, dir, entries);

        bool skipSync = false;
        if (pushOnly) {
            // Check if there were any changes locally:
            bool localChanges = false;
            for (auto key : entries.keys()) {
                auto entry = entries[key];
                if (entry.lastModDate != entry.previousLasModDate) {
                    localChanges = true;
                }
            }
            // If there were local changes, get the current list of
            // remote entries; this is to avoid sync issues if clients
            // run in parallel:
            if (localChanges) {
                result = result && mergeRemoteInfoWithSyncList(entries, dir);
            } else {
                skipSync = true;
            }
        } else {
            result = result && mergeRemoteInfoWithSyncList(entries, dir);
        }

        qCDebug(log) << "Synchronizing" <<
                                       QDir::cleanPath(this->directory()
                                                       + "/" + directory);
        emit debug(tr("Synchronizing '%1'")
                   .arg(QDir::cleanPath(this->directory()
                                        + "/" + directory)));

        if (!skipSync) {
            for (auto entry : entries) {
                if (m_stopRequested) {
                    break;
                }
                if (!entry.etag.isNull() && entry.etag != entry.previousEtag) {
                    if (skipEntry(entry, Upload, directoryFilter)) {
                        qCDebug(log) << "Ignoring"
                                                    << entry.path();
                        emit debug(tr("Ignoring file %1").arg(entry.path()));
                        continue;
                    }
                    if (entry.remoteType == Directory) {
                        _changedDirs.insert(entry.entry);
                    }
                    result = result && pullEntry(entry, db);
                } else if (entry.etag.isNull() && !entry.previousEtag.isNull()) {
                    if (skipEntry(entry, Upload, directoryFilter)) {
                        qCDebug(log) << "Ignoring"
                                                    << entry.path();
                        emit debug(tr("Ignoring file %1").arg(entry.path()));
                        continue;
                    }
                    result = result && removeLocalEntry(entry, db);
                } else if (!entry.lastModDate.isNull() &&
                           entry.lastModDate != entry.previousLasModDate) {
                    if (skipEntry(entry, Upload, directoryFilter)) {
                        qCDebug(log) << "Ignoring"
                                                    << entry.path();
                        emit debug(tr("Ignoring file %1").arg(entry.path()));
                        continue;
                    }
                    result = result && pushEntry(entry, db);
                } else if (entry.lastModDate.isNull() &&
                           !entry.previousLasModDate.isNull()) {
                    if (skipEntry(entry, Upload, directoryFilter)) {
                        qCDebug(log) << "Ignoring"
                                                    << entry.path();
                        emit debug(tr("Ignoring file %1").arg(entry.path()));
                        continue;
                    }
                    result = result && removeRemoteEntry(entry, db);
                }
            }
        } else {
            qCDebug(log) << "Skipping sync of " << directory
                                    << "as there were no local changes and we"
                                    << "have been asked to push only";
            emit debug(tr("Skipping sync of directory '%1' as there were no "
                          "local changes and we have been asked to push only")
                       .arg(directory));
        }
        db.close();
    }
    if (!dbConnName.isNull()) {
        QSqlDatabase::removeDatabase(dbConnName);
    }
    if (changedDirs != nullptr) {
        *changedDirs = _changedDirs;
    }
    return result;
}


/**
 * @brief Merge a sync entry list with local file data.
 */
void WebDAVClient::mergeLocalInfoWithSyncList(
        QDir &d, const QString& dir, SyncEntryMap& entries)
{
    for (auto entry : d.entryList(
             QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
        QFileInfo fi(d.absoluteFilePath(entry));
        auto file = entries.value(entry, SyncEntry());
        file.entry = entry;
        file.parent = dir;
        file.localType = fi.isDir() ? Directory : File;
        file.lastModDate = fi.lastModified();
        entries[file.entry] = file;
    }
}


/**
 * @brief Merge a sync entry list with remote file data.
 */
bool WebDAVClient::mergeRemoteInfoWithSyncList(
        SyncEntryMap& entries, const QString& dir)
{
    bool ok;
    auto remoteEntries = entryList(dir, &ok);
    if (ok) {
        for (auto entry : remoteEntries) {
            if (entry.name != ".") {
                auto file = entries[entry.name];
                file.parent = dir;
                file.entry = entry.name;
                file.remoteType = entry.type == Directory ?
                            Directory : File;
                file.etag = entry.etag;
                entries[file.entry] = file;
            }
        }
    } else {
        qCWarning(log) << "Failed to get entry list for"
                                      << dir;
        emit warning(tr("Failed to get entry list for '%1'").arg(dir));
        for (auto entry : entries.keys()) {
            auto &e = entries[entry];
            e.etag = e.previousEtag;
        }
    }
    return ok;
}


/**
 * @brief Pull an entry from the server.
 */
bool WebDAVClient::pullEntry(
        WebDAVClient::SyncEntry &entry, QSqlDatabase &db)
{
    qCDebug(log) << "Pulling" << entry.path();
    emit debug(tr("Pulling '%1'").arg(entry.path()));
    bool result = false;
    if (entry.remoteType == File) {
        // Pull a file
        if (entry.localType == Directory) {
            qCWarning(log)
                    << "Pull conflict: Cannot pull file"
                    << entry.entry << "from" << entry.parent
                    << "because a local directory with that "
                       "name already exists";
            emit error(tr("Pull conflict: Cannot pull file '%1' from "
                          "'%2'  because a local directory with than "
                          "name already exists")
                       .arg(entry.entry)
                       .arg(entry.parent));
        } else {
            if (download(entry.parent + "/" + entry.entry)) {
                QFileInfo fi(this->directory() + "/" + entry.parent
                             + "/" + entry.entry);
                entry.lastModDate = fi.lastModified();
                insertSyncDBEntry(db, entry);
                result = true;
            }
        }
    } else if (entry.remoteType == Directory) {
        // Pull a directory
        if (entry.localType == File) {
            qCWarning(log)
                    << "Pull conflict: Cannot pull directory"
                    << entry.entry << "from" << entry.parent
                    << "because a file with that name already "
                       "exists locally";
            emit error(tr("Pull conflict: Cannot pull directory '%1' from '%2' "
                          "because a file with that name already exists "
                          "locally")
                       .arg(entry.entry)
                       .arg(entry.parent));
        } else if (entry.localType == Invalid) {
            if (QDir(this->directory() + "/" + entry.parent).mkdir(
                        entry.entry)) {
                QFileInfo fi(this->directory() + "/" + entry.parent
                             + "/" + entry.entry);
                entry.lastModDate = fi.lastModified();
                insertSyncDBEntry(db, entry);
                result = true;
            }
        } else if (entry.localType == Directory) {
            QFileInfo fi(this->directory() + "/" + entry.parent
                         + "/" + entry.entry);
            entry.lastModDate = fi.lastModified();
            insertSyncDBEntry(db, entry);
            result = true;
        }
    } else {
        // Should not happen...
        qCWarning(log) << "Cannot pull remote entry "
                                         "of type Unknown";
        emit warning(tr("Cannot pull remote entry of type Unknown"));
        result = false;
    }
    return result;
}


/**
 * @brief Remove a local file or directory.
 */
bool WebDAVClient::removeLocalEntry(
        WebDAVClient::SyncEntry &entry, QSqlDatabase &db)
{
    qCDebug(log) << "Removing" << entry.path() << "locally";
    emit debug(tr("Removing '%1' locally").arg(entry.path()));
    bool result = false;
    if (entry.localType == File) {
        if (!QDir(directory() + "/" + entry.parent).remove(entry.entry)) {
            qCWarning(log) << "Failed to remove local file"
                                          << entry.entry << "from"
                                          << entry.parent;
            emit warning(tr("Failed to remove local file '%1' "
                            "from '%2'")
                         .arg(entry.entry)
                         .arg(entry.parent));
        } else {
            removeFileFromSyncDB(db, entry);
            result = true;
        }
    } else if (entry.localType == Directory) {
        if (rmLocalDir(directory() + "/" + entry.path(), 2)) {
            removeDirFromSyncDB(db, entry);
            result = true;
        } else {
            qCWarning(log) << "Failed to remove local"
                                          << "directory" << entry.path();
            emit warning(tr("Failed to remove local directory '%1'")
                         .arg(entry.path()));
        }
    } else {
        // Should not happen
        qCWarning(log) << "Bad sync entry type of entry"
                                      << entry.entry << "in" << entry.parent
                                      << "in removeLocalEntry";
        emit error(tr("Bad sync entry type of entry '%1' in '%2' "
                      "when removing local entry")
                   .arg(entry.entry)
                   .arg(entry.parent));
    }
    return result;
}


/**
 * @brief Push an entry to the server.
 */
bool WebDAVClient::pushEntry(
        WebDAVClient::SyncEntry &entry, QSqlDatabase &db)
{
    qCDebug(log) << "Pushing" << entry.path();
    emit debug(tr("Pushing '%1'").arg(entry.path()));
    bool result = false;
    if (entry.localType == Directory) {
        if (entry.remoteType == File) {
            qCWarning(log)
                    << "Conflict: Cannot push directory" << entry.path()
                    << "as a file with that name exists on the remote";
            emit warning(tr("Push conflict: Cannot push directory '%1' "
                            "as a file with that name exists on the remote")
                         .arg(entry.path()));
        } else if (entry.remoteType == Directory) {
            insertSyncDBEntry(db, entry);
            result = true;
        } else if (entry.remoteType == Invalid) {
            if (mkdir(entry.path(), &entry.etag)) {
                insertSyncDBEntry(db, entry);
                result = true;
            }
        }
    } else if (entry.localType == File) {
        if (entry.remoteType == Directory) {
            qCWarning(log)
                    << "Conflict: Cannot push local file" << entry.path()
                    << "because a directory with that name exists remotely";
            emit warning(tr("Push conflict: Cannot push local file '%1' "
                            "because a directory with that name exists "
                            "remotely")
                         .arg(entry.path()));
        } else {
            if (upload(entry.path(), &entry.etag)) {
                insertSyncDBEntry(db, entry);
                result = true;
            }
        }
    } else if (entry.localType == Invalid) {
        qCWarning(log) << "Unexpected local type of entry"
                                      << entry.path();
        emit error(tr("Unexpected local type of entry '%1'")
                   .arg(entry.path()));
    }
    return result;
}


/**
 * @brief Remove an entry on the server.
 */
bool WebDAVClient::removeRemoteEntry(
        WebDAVClient::SyncEntry &entry, QSqlDatabase &db)
{
    qCDebug(log) << "Removing" << entry.path() << "remotely";
    emit debug(tr("Removing remote entry '%1'").arg(entry.path()));
    bool result = false;
    if (deleteEntry(entry.path())) {
        if (entry.localType == Directory) {
            removeDirFromSyncDB(db, entry);
        } else {
            removeFileFromSyncDB(db, entry);
        }
        result = true;
    }
    return result;
}

bool WebDAVClient::skipEntry(
        const WebDAVClient::SyncEntry &entry,
        WebDAVClient::SyncStepDirection direction,
        const QRegularExpression &dirFilter)
{
    switch (direction) {
    case Download:
        switch (entry.remoteType) {
        case Directory:
            return !dirFilter.match(entry.entry).hasMatch();
        case File:
            return entry.entry.startsWith(".");
        default:
            break;
        }
        break;
    case Upload:
        switch (entry.localType) {
        case Directory:
            return !dirFilter.match(entry.entry).hasMatch();
        case File:
            return entry.entry.startsWith(".");
        default:
            break;
        }
        break;
    default:
        break;
    }
    return false;
}


/**
 * @brief Get the current etag of a file or directory.
 *
 * Get the etag of the file or directory identified by the @p filename.
 * The file name is relative to the remote directory set.
 * If the etag cannot be retrieved, an empty string is returned.
 */
QString WebDAVClient::etag(const QString& filename)
{
    QString result;
    auto path = m_remoteDirectory + "/" + filename;
    auto reply = etagRequest(path);
    waitForReplyToFinish(reply);
    auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (code.toInt() == HTTPStatusCode::WebDAVMultiStatus) {
        auto entryList = parseEntryList(baseUrl(), path, reply->readAll());
        if (entryList.length() == 1) {
            auto entry = entryList.at(0);
            if (entry.name == ".") {
                result = entry.etag;
            }
        }
    }
    return result;
}


/**
 * @brief Create a proper path relative to the set directory.
 *
 * Create a path from the given @p path. The returned path will be relative
 * to the currently set local/remote path.
 */
QString WebDAVClient::mkpath(const QString &path)
{
    auto result = QDir::cleanPath(path);
    if (result.startsWith("/")) {
        result = result.mid(1);
    }
    return result;
}


/**
 * @brief Split a path.
 *
 * This helper method splits up a path into a parent path and the trailing
 * file (or directory) name part and return it as a tuple.
 */
std::tuple<QString, QString> WebDAVClient::splitpath(const QString &path)
{
    std::tuple<QString, QString> result;
    auto p = mkpath(path);
    auto list = p.split("/");
    if (list.length() == 1) {
        result = std::make_tuple(QString(""), list[0]);
    } else if (list.length() > 1) {
        auto last = list.takeLast();
        result = std::make_tuple(list.join("/"), last);
    }
    return result;
}


/**
 * @brief Get the URL as a string with a trailing slash.
 */
QString WebDAVClient::urlString() const
{
    auto result = baseUrl().toString();
    if (!result.endsWith("/")) {
        result += "/";
    }
    return result;
}




QNetworkReply* WebDAVClient::listDirectoryRequest(
        const QString& directory)
{
    /*
     curl -i -X PROPFIND http://admin:admin@localhost:8080/remote.php/webdav/ \
        --upload-file - -H "Depth: 1" <<END
     <?xml version="1.0"?>
     <a:propfind xmlns:a="DAV:">
     <a:prop><a:resourcetype/></a:prop>
     </a:propfind>
     END
    */

    QByteArray requestData = "<?xml version=\"1.0\"?>"
                             "<a:propfind xmlns:a=\"DAV:\">"
                             "<a:prop>"
                             "<a:resourcetype/>"
                             "<a:getetag/>"
                             "</a:prop>"
                             "</a:propfind>";
    QNetworkRequest request;
    auto url = QUrl(urlString() + mkpath(directory));
    url.setUserName(m_username);
    url.setPassword(m_password);
    request.setUrl(url);
    request.setRawHeader("Depth", "1");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      requestData.size());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "text/xml; charset=utf-8");
    auto buffer = new QBuffer;
    buffer->setData(requestData);
    auto reply = m_networkAccessManager->sendCustomRequest(
                request, "PROPFIND", buffer);
    connect(qApp, &QCoreApplication::aboutToQuit,
            reply, &QNetworkReply::abort);
    buffer->setParent(reply);
    prepareReply(reply);
    return reply;
}

QNetworkReply* WebDAVClient::etagRequest(const QString& filename)
{
    /*
     curl -i -X PROPFIND http://admin:admin@localhost:8080/remote.php/webdav/ \
        --upload-file - -H "Depth: 0" <<END
     <?xml version="1.0"?>
     <a:propfind xmlns:a="DAV:">
     <a:prop><a:getetag/></a:prop>
     </a:propfind>
     END
    */

    QByteArray requestData = "<?xml version=\"1.0\"?>"
                             "<a:propfind xmlns:a=\"DAV:\">"
                             "<a:prop>"
                             "<a:getetag/>"
                             "</a:prop>"
                             "</a:propfind>";
    QNetworkRequest request;
    auto url = QUrl(urlString() + mkpath(filename));
    url.setUserName(m_username);
    url.setPassword(m_password);
    request.setUrl(url);
    request.setRawHeader("Depth", "0");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      requestData.size());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "text/xml; charset=utf-8");
    auto buffer = new QBuffer;
    buffer->setData(requestData);
    auto reply = m_networkAccessManager->sendCustomRequest(
                request, "PROPFIND", buffer);
    connect(qApp, &QCoreApplication::aboutToQuit,
            reply, &QNetworkReply::abort);
    buffer->setParent(reply);
    prepareReply(reply);
    return reply;
}

QNetworkReply*WebDAVClient::createDirectoryRequest(
        const QString& directory)
{
    /*
     curl -X MKCOL 'http://admin:admin@localhost:8080/remote.php/webdav/example'
    */
    QNetworkRequest request;
    auto url = QUrl(urlString() + mkpath(directory));
    url.setUserName(m_username);
    url.setPassword(m_password);
    request.setUrl(url);
    auto reply = m_networkAccessManager->sendCustomRequest(
                request, "MKCOL");
    connect(qApp, &QCoreApplication::aboutToQuit,
            reply, &QNetworkReply::abort);
    prepareReply(reply);
    return reply;
}

WebDAVClient::EntryList WebDAVClient::parseEntryList(
        const QUrl &baseUrl, const QString& directory, const QByteArray& reply)
{
    EntryList result;
    QDomDocument doc;
    QString errorMsg;
    int errorLine;
    if (doc.setContent(reply, &errorMsg, &errorLine)) {
        result = parsePropFindResponse(baseUrl, doc, directory);
    } else {
        qCWarning(log) << "Failed to parse WebDAV response:"
                                      << errorMsg << "in line" << errorLine;
    }
    return result;
}

WebDAVClient::EntryList WebDAVClient::parsePropFindResponse(
        const QUrl &baseUrl, const QDomDocument& response, const QString& directory)
{
    EntryList result;
    auto baseDir = QDir::cleanPath(baseUrl.path() + "/" + directory);
    auto root = response.documentElement();
    auto rootTagName = root.tagName();
    if (rootTagName == "d:multistatus") {
        auto resp = root.firstChildElement("d:response");
        while (resp.isElement()) {
            auto entry = parseResponseEntry(resp, baseDir);
            if (entry.type != Invalid) {
                result << entry;
            }
            resp = resp.nextSiblingElement("d:response");
        }
    } else {
        qCWarning(log) << "Received invalid WebDAV response from"
                                         "server starting with element"
                                      << rootTagName;
    }
    return result;
}

WebDAVClient::Entry WebDAVClient::parseResponseEntry(
        const QDomElement& element, const QString& baseDir)
{
    auto type = File;
    QString etag;

    auto propstats = element.elementsByTagName("d:propstat");
    for (int i = 0; i < propstats.length(); ++i) {
        auto propstat = propstats.at(i).toElement();
        auto status = propstat.firstChildElement("d:status");
        if (status.text().endsWith("200 OK")) {
            auto prop = propstat.firstChildElement("d:prop");
            auto child = prop.firstChildElement();
            while (child.isElement()) {
                if (child.tagName() == "d:resourcetype") {
                    if (child.firstChildElement().tagName() == "d:collection") {
                        type = Directory;
                    }
                } else if (child.tagName() == "d:getetag") {
                    etag = child.text();
                } else {
                    qCWarning(log) << "Unknown DAV Property:"
                                                  << child.tagName();
                }
                child = child.nextSiblingElement();
            }
        } else {
            qCWarning(log) << "Properties not retrieved -"
                                          << status.text();
        }
    }


    QString path = QByteArray::fromPercentEncoding(
                element.firstChildElement("d:href").text().toUtf8());

    path = QDir(baseDir).relativeFilePath(path);
    Entry result;
    result.type = type;
    result.etag = etag;
    result.name = path;
    return result;
}

void WebDAVClient::prepareReply(QNetworkReply* reply) const
{
    connect(reply, &QNetworkReply::sslErrors,
            [=](QList<QSslError> errors) {
        for (auto error : errors) {
            qCWarning(log) << error.errorString();
            emit this->error(tr("There was an SSL error: %1")
                       .arg(error.errorString()));
            if (!m_disableCertificateCheck) {
                emit syncError(tr("Problem establishing a secure connection to "
                                  "the server: %1").arg(error.errorString()));
            }
        }
        if (m_disableCertificateCheck) {
            reply->ignoreSslErrors(errors);
        }
    });
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            [=](QNetworkReply::NetworkError error) {
        qCWarning(log) << "Network error:" << error;
        emit this->error(tr("There was a network error: %1")
                         .arg(QVariant::fromValue(error).toString()));
        if (error == QNetworkReply::AuthenticationRequiredError) {
            emit syncError(tr("Authentication failed. Please check you username "
                              "and password."));
        }
    });
}


/**
 * @brief Waits for the @p reply to finish.
 *
 * This is a helper method which waits for the network reply to finish, i.e.
 * either the reply's finished() signal is emitted or there was no up- or
 * download progress within a given interval.
 */
void WebDAVClient::waitForReplyToFinish(QNetworkReply* reply)
{
    Q_CHECK_PTR(reply);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            &loop, SLOT(quit()));
    loop.exec();
}


/**
 * @brief Open the SyncDB database.
 */
QSqlDatabase WebDAVClient::openSyncDb() {
    const QString& localDir = this->directory();
    auto dbPath = QDir::cleanPath(localDir + "/.otlwebdavsync.db");
    auto db = QSqlDatabase::addDatabase("QSQLITE", dbPath);
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        qCWarning(log) << "Failed to open database:"
                                     << db.lastError().text();
        return db;
    }
    QSqlQuery query(db);
    query.prepare("CREATE TABLE IF NOT EXISTS "
                  "version (key string PRIMARY KEY, value);");
    if (!query.exec()) {
        qCWarning(log) << "Failed to create version table:"
                                     << query.lastError().text();
    }
    query.prepare("SELECT value FROM version WHERE key == 'version';");
    int version = 0;
    if (query.exec()) {
        if (query.first()) {
            auto record = query.record();
            version = record.value("value").toInt();
        }
    } else {
        qCWarning(log) << "Failed to get version of sync DB:"
                                      << query.lastError().text();
    }
    if (version == 0) {
        query.prepare("CREATE TABLE files ("
                      "`parent` string, "
                      "`entry` string NOT NULL, "
                      "`modificationDate` date not null, "
                      "`etag` string not null, "
                      "PRIMARY KEY(`parent`, `entry`)"
                      ");");
        if (!query.exec()) {
            qCWarning(log) << "Failed to create files table:"
                                         << query.lastError().text();
        }
        query.prepare("INSERT OR REPLACE INTO version(key, value) "
                      "VALUES ('version', 1);");
        if (!query.exec()) {
            qCWarning(log) << "Failed to insert version into DB:"
                                         << query.lastError().text();
        }
    }
    return db;
}


/**
 * @brief Insert a single entry into the SyncDB.
 *
 * This inserts the entry into the SyncDB. The current modification date and
 * etag will be stored in the DB.
 */
void WebDAVClient::insertSyncDBEntry(
        QSqlDatabase &db, const WebDAVClient::SyncEntry &entry) {
    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO files "
                  "(parent, entry, modificationDate, etag) "
                  "VALUES (?, ?, ?, ?);");
    query.addBindValue(entry.parent);
    query.addBindValue(entry.entry);
    query.addBindValue(entry.lastModDate);
    query.addBindValue(entry.etag);
    if (!query.exec()) {
        qCWarning(log) << "Failed to insert SyncDB entry:"
                                      << query.lastError().text();
    }
}


/**
 * @brief Get entries for given directory.
 *
 * This retrieves a list of entries from the last sync run
 * for the given parent directory. The returned entries
 * will have their previousLastModDate and previousEtag entries
 * set.
 */
WebDAVClient::SyncEntryMap WebDAVClient::findSyncDBEntries(
        QSqlDatabase &db, const QString &parent) {
    QMap<QString, SyncEntry> result;
    QSqlQuery query(db);
    query.prepare("SELECT parent, entry, modificationDate, etag "
                  "FROM files WHERE parent = ?;");
    query.addBindValue(parent);
    if (query.exec()) {
        while (query.next()) {
            SyncEntry entry;
            auto record = query.record();
            entry.parent = record.value("parent").toString();
            entry.entry = record.value("entry").toString();
            entry.previousLasModDate = record.value("modificationDate")
                    .toDateTime();
            entry.previousEtag = record.value("etag").toString();
            result[entry.entry] = entry;
        }
    } else {
        qCWarning(log) << "Failed to get sync entries from DB:"
                                      << query.lastError().text();
    }
    return result;
}


/**
 * @brief Remove a directory from the SyncDB.
 */
void WebDAVClient::removeDirFromSyncDB(
        QSqlDatabase &db, const SyncEntry& entry) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM files "
                  "WHERE parent LIKE '%' || ? OR (parent = ? AND entry = ?);");
    query.addBindValue(entry.path());
    query.addBindValue(entry.parent);
    query.addBindValue(entry.entry);
    if (!query.exec()) {
        qCWarning(log) << "Failed to delete directory from "
                                         "sync DB:"
                                      << query.lastError().text();
    }
}


/**
 * @brief Remove a single entry from the SyncDB.
 */
void WebDAVClient::removeFileFromSyncDB(
        QSqlDatabase &db, const WebDAVClient::SyncEntry &entry) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM files WHERE parent = ? AND entry = ?;");
    query.addBindValue(entry.parent);
    query.addBindValue(entry.entry);
    if (!query.exec()) {
        qCWarning(log) << "Failed to remove entry from sync DB:"
                                      << query.lastError().text();
    }
}


/**
 * @brief Remove a directory.
 *
 * This removes the directory @p dir. All files and directories in that
 * directory are removed first, before the actual directory is removed.
 * If @p maxDepth greater than zero, rmLocalDir() is called for all
 * sub-directories with a maxDepth - 1. Hence, this method can be used to
 * reduce the total depth up to which the deletion is executed.
 */
bool WebDAVClient::rmLocalDir(const QString &dir, int maxDepth) {
    if (!dir.isEmpty()) {
        QDir d(dir);
        if (d.exists()) {
            for (auto entry : d.entryList(QDir::Dirs | QDir::Files |
                                          QDir::NoDotAndDotDot)) {
                QFileInfo fi(d.absoluteFilePath(entry));
                if (fi.isDir() && maxDepth > 0) {
                    rmLocalDir(fi.absoluteFilePath(), maxDepth - 1);
                }
                if (fi.isDir()) {
                    d.rmdir(entry);
                } else {
                    d.remove(entry);
                }
            }
            auto dirName = d.dirName();
            if (d.cdUp()) {
                return d.rmdir(dirName);
            }
        }
    }
    return false;
}
