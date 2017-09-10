#include "webdavsynchronizer.h"

#include "webdavclient.h"

#include "datamodel/library.h"

#include <QBuffer>
#include <QDir>
#include <QDomDocument>
#include <QEventLoop>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTemporaryFile>
#include <QTimer>


Q_LOGGING_CATEGORY(webDAVSynchronizer,
                   "net.rpdev.opentodolist.WebDAVSynchronizer",
                   QtDebugMsg)



WebDAVSynchronizer::WebDAVSynchronizer(QObject* parent) :
    Synchronizer(parent),
    m_remoteDirectory(),
    m_disableCertificateCheck(false),
    m_username(),
    m_password(),
    m_findExistingEntriesWatcher()
{
    connect(&m_findExistingEntriesWatcher,
            &QFutureWatcher<QVariantList>::finished, [=]() {
        setExistingLibraries(m_findExistingEntriesWatcher.result());
    });
}

WebDAVSynchronizer::~WebDAVSynchronizer()
{
    if (m_findExistingEntriesWatcher.isRunning()) {
        m_findExistingEntriesWatcher.waitForFinished();
    }
}

void WebDAVSynchronizer::validate()
{
    beginValidation();
    auto dav = createDAVClient(this);
    auto reply = dav->listDirectoryRequest("/");
    reply->setParent(this);
    connect(reply, &QNetworkReply::finished, [=]() {
        auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (code == HTTPStatusCode::WebDAVMultiStatus) {
            endValidation(true);
        } else {
            endValidation(false);
        }
        reply->deleteLater();
        dav->deleteLater();
    });
}

void WebDAVSynchronizer::synchronize()
{
    if (!directory().isEmpty() && !synchronizing()) {
        auto dav = createDAVClient(this);
        setSynchronizing(true);
        // Sync the top level directory:
        QSet<QString> changedYearDirs;
        dav->syncDirectory("/",
                           QRegularExpression("\\d\\d\\d\\d"),
                           false,
                           &changedYearDirs);
        // Sync the year directory:
        QDir dir(directory());
        for (auto yearDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QSet<QString> changedMonthDirs;
            dav->syncDirectory("/" + yearDir,
                               QRegularExpression("\\d\\d?"),
                               !changedYearDirs.contains(yearDir),
                               &changedMonthDirs);
            QDir ydir(dir.absoluteFilePath(yearDir));
            for (auto monthDir : ydir.entryList(
                     QDir::Dirs | QDir::NoDotAndDotDot)) {
                dav->syncDirectory("/" + yearDir + "/" + monthDir,
                                   QRegularExpression(),
                                   !changedMonthDirs.contains(monthDir));
            }
        }
        setSynchronizing(false);
        delete dav;
    }
}



/**
 * @brief Search for existing libraries.
 *
 * This method will search for existing libraries in the current
 * remote directory and the OpenTodoList sub-folder (relative to the
 * remote directory). The
 */
void WebDAVSynchronizer::findExistingLibraries()
{
    auto baseUrl = this->baseUrl();
    auto username = this->username();
    auto password = this->password();
    auto disableCertificateCheck = this->disableCertificateCheck();
    auto remoteDirectory = this->remoteDirectory();
    auto directory = this->directory();

    m_findExistingEntriesWatcher.setFuture(
                QtConcurrent::run([=]() -> QVariantList {
        WebDAVClient client;
        client.setBaseUrl(baseUrl);
        client.setUsername(username);
        client.setPassword(password);
        client.setDisableCertificateCheck(disableCertificateCheck);
        client.setRemoteDirectory(remoteDirectory);
        client.setDirectory(directory);

        QStringList dirsToCheck;
        for (auto baseDir : QStringList({"/", "/OpenTodoList"})) {
            auto entryList = client.entryList(baseDir);
            for (auto entry : entryList) {
                if (entry.type == WebDAVClient::Directory &&
                        entry.name.endsWith(".otl")) {
                    dirsToCheck.append(client.mkpath(baseDir + "/" +
                                                     entry.name));
                }
            };
        }
        QVariantList result;
        for (auto dir : dirsToCheck) {
            QByteArray json;
            QBuffer buffer(&json);
            buffer.open(QIODevice::WriteOnly);
            if (client.download(dir + "/" + Library::LibraryFileName,
                                &buffer)) {
                if (!json.isEmpty()) {
                    Library lib;
                    lib.fromJson(json);
                    SynchronizerExistingLibrary library;
                    library.setName(lib.name());
                    library.setPath(dir);
                    result << QVariant::fromValue(library);
                }
            }
        }
        return result;
    }));
}


QVariantMap WebDAVSynchronizer::toMap() const
{
    auto result = Synchronizer::toMap();
    result["username"] = m_username;
    result["remoteDirectory"] = m_remoteDirectory;
    result["disableCertificateCheck"] = m_disableCertificateCheck;
    return result;
}

void WebDAVSynchronizer::fromMap(const QVariantMap& map)
{
    m_username = map.value("username").toString();
    m_remoteDirectory = map.value("remoteDirectory").toString();
    m_disableCertificateCheck = map.value("disableCertificateCheck").toBool();
    Synchronizer::fromMap(map);
}

QString WebDAVSynchronizer::remoteDirectory() const
{
    return m_remoteDirectory;
}

void WebDAVSynchronizer::setRemoteDirectory(const QString& remoteDirectory)
{
    if (m_remoteDirectory != remoteDirectory) {
        m_remoteDirectory = remoteDirectory;
        emit remoteDirectoryChanged();
    }
}

bool WebDAVSynchronizer::disableCertificateCheck() const
{
    return m_disableCertificateCheck;
}

void WebDAVSynchronizer::setDisableCertificateCheck(bool disableCertificateCheck)
{
    if (m_disableCertificateCheck != disableCertificateCheck) {
        m_disableCertificateCheck = disableCertificateCheck;
        emit disableCertificateCheckChanged();
    }
}

QString WebDAVSynchronizer::username() const
{
    return m_username;
}

void WebDAVSynchronizer::setUsername(const QString& username)
{
    if (m_username != username) {
        m_username = username;
        emit usernameChanged();
    }
}

QString WebDAVSynchronizer::password() const
{
    return m_password;
}

void WebDAVSynchronizer::setPassword(const QString& password)
{
    if (m_password != password) {
        m_password = password;
        emit passwordChanged();
    }
}


/**
 * @brief Create a DAV client.
 *
 * This creates and returns a new WebDAVClient which belongs to the
 * @p parent. The client will have the settings currently set in the
 * WebDAVSynchronizer.
 *
 * @note Ownership of the object goes to the caller.
 */
WebDAVClient* WebDAVSynchronizer::createDAVClient(QObject *parent)
{
    auto result = new WebDAVClient(parent);
    result->setBaseUrl(baseUrl());
    result->setUsername(username());
    result->setPassword(password());
    result->setDisableCertificateCheck(disableCertificateCheck());
    result->setRemoteDirectory(remoteDirectory());
    result->setDirectory(directory());
    return result;
}
