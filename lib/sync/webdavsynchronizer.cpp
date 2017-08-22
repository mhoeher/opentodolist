#include "webdavsynchronizer.h"

#include <QBuffer>
#include <QDir>
#include <QDomDocument>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>


Q_LOGGING_CATEGORY(webDAVSynchronizer,
                   "net.rpdev.opentodolist.WebDAVSynchronizer",
                   QtDebugMsg)

enum {
    WebDAVMultiGetResponseCode = 207,
    HTTPOkCode = 200,
    HTTPCreatedCode = 201,
    HTTPNoContentCode = 204
};



WebDAVSynchronizer::WebDAVSynchronizer(QObject* parent) :
    Synchronizer(parent),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_remoteDirectory(),
    m_disableCertificateCheck(false),
    m_username(),
    m_password()
{

}

WebDAVSynchronizer::~WebDAVSynchronizer()
{

}

void WebDAVSynchronizer::validate()
{
    beginValidation();
    auto reply = listDirectoryRequest("/");
    reply->setParent(this);
    connect(reply, &QNetworkReply::finished, [=]() {
        auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (code == WebDAVMultiGetResponseCode) {
            endValidation(true);
        } else {
            endValidation(false);
        }
        reply->deleteLater();
    });
}

void WebDAVSynchronizer::synchronize()
{
    // TODO: Implement me
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
 * @brief Get the list of entries on the given @p directory.
 *
 * This retrieves the contents of the @p directory (which is relative to the
 * remoteDirectory set in the WebDAVSynchronizer itself).
 */
WebDAVSynchronizer::EntryList WebDAVSynchronizer::entryList(const QString& directory, bool *ok)
{
    // TODO: Implement me
    Q_UNUSED(directory);
    EntryList result;
    auto dir = this->remoteDirectory() + "/" + directory;
    dir = QDir::cleanPath(dir);
    auto reply = listDirectoryRequest(dir);
    bool status = false;
    connect(reply, &QNetworkReply::finished, [&]() {
        auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (code == WebDAVMultiGetResponseCode) {
            result = parseEntryList(dir, reply->readAll());
            status = true;
        }
    });
    waitForReplyToFinish(reply);
    if (ok != nullptr) {
        *ok = status;
    }
    return result;
}

bool WebDAVSynchronizer::download(const QString& filename)
{
    return false;
}

bool WebDAVSynchronizer::upload(const QString& filename)
{
    auto result = false;
    auto file = new QFile(directory() + "/" + filename);
    if (file->open(QIODevice::ReadOnly)) {
        QNetworkRequest request;
        auto url = QUrl(baseUrl().toString() +
                        QDir::cleanPath("/" + remoteDirectory() + "/" + filename));
        url.setUserName(username());
        url.setPassword(password());
        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentLengthHeader,
                          file->size());
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          "application/octet-stream");
        auto reply = m_networkAccessManager->put(request, file);
        file->setParent(reply);
        waitForReplyToFinish(reply);
        auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (code == HTTPOkCode || code == HTTPCreatedCode ||
                code == HTTPNoContentCode) {
            result = true;
        } else {
            qCWarning(webDAVSynchronizer) << "Upload failed with code" << code;
        }
    } else {
        qCWarning(webDAVSynchronizer) << "Failed to open" << filename
                                      << "for reading:" << file->errorString();
        delete file;
    }
    return result;
}

bool WebDAVSynchronizer::mkdir(const QString& dirname)
{
    auto result = false;
    auto reply = createDirectoryRequest(dirname);
    connect(reply, &QNetworkReply::finished, [&]() {
        auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        result = code == HTTPCreatedCode;
    });
    waitForReplyToFinish(reply);
    return result;
}


QNetworkReply* WebDAVSynchronizer::listDirectoryRequest(const QString& directory)
{
    /*
     curl -i -X PROPFIND http://admin:admin@localhost:8080/remote.php/webdav/ --upload-file - -H "Depth: 1" <<END
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
    auto baseUrl = this->baseUrl();
    auto dir = QDir::cleanPath(directory);
    QUrl url(baseUrl.toString() + "/" + dir);
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
    buffer->setParent(reply);
    prepareReply(reply);
    return reply;
}

QNetworkReply*WebDAVSynchronizer::createDirectoryRequest(
        const QString& directory)
{
    /*
     curl -X MKCOL 'http://admin:admin@localhost:8080/remote.php/webdav/example'
    */
    QNetworkRequest request;
    auto baseUrl = this->baseUrl();
    auto dir = QDir::cleanPath(directory);
    QUrl url(baseUrl.toString() + "/" + dir);
    url.setUserName(m_username);
    url.setPassword(m_password);
    request.setUrl(url);
    auto reply = m_networkAccessManager->sendCustomRequest(
                request, "MKCOL");
    prepareReply(reply);
    return reply;
}

WebDAVSynchronizer::EntryList WebDAVSynchronizer::parseEntryList(
        const QString& directory, const QByteArray& reply)
{
    EntryList result;
    QDomDocument doc;
    QString errorMsg;
    int errorLine;
    if (doc.setContent(reply, &errorMsg, &errorLine)) {
        result = parsePropFindResponse(doc, directory);
    } else {
        qCWarning(webDAVSynchronizer) << "Failed to parse WebDAV response:"
                                      << errorMsg << "in line" << errorLine;
    }
    return result;
}

WebDAVSynchronizer::EntryList WebDAVSynchronizer::parsePropFindResponse(
        const QDomDocument& response, const QString& directory)
{
    EntryList result;
    auto baseUrl = this->baseUrl();
    auto baseDir = QDir::cleanPath(baseUrl.path() + "/" + directory);
    auto root = response.documentElement();
    if (root.tagName() == "multistatus") {
        auto resp = root.firstChildElement("response");
        while (resp.isElement()) {
            auto entry = parseResponseEntry(resp, baseDir);
            if (entry.type != Invalid) {
                result << entry;
            }
            resp = resp.nextSiblingElement("response");
        }
    }
    return result;
}

WebDAVSynchronizer::Entry WebDAVSynchronizer::parseResponseEntry(
        const QDomElement& element, const QString& baseDir)
{
    auto type = File;
    QString etag;

    auto propstats = element.elementsByTagName("propstat");
    for (int i = 0; i < propstats.length(); ++i) {
        auto propstat = propstats.at(i).toElement();
        auto status = propstat.firstChildElement("status");
        if (status.text().endsWith("200 OK")) {
            auto prop = propstat.firstChildElement("prop");
            auto child = prop.firstChildElement();
            while (child.isElement()) {
                if (child.tagName() == "resourcetype") {
                    if (child.firstChildElement().tagName() == "collection") {
                        type = Directory;
                    }
                } else if (child.tagName() == "getetag") {
                    etag = child.text();
                } else {
                    qCWarning(webDAVSynchronizer) << "Unknown DAV Property:"
                                                  << child.tagName();
                }
                child = child.nextSiblingElement();
            }
        } else {
            qCWarning(webDAVSynchronizer) << "Properties not retrieved -"
                                          << status.text();
        }
    }


    auto path = element.firstChildElement("href").text();
    path = QDir(baseDir).relativeFilePath(path);
    Entry result;
    result.type = type;
    result.etag = etag;
    result.name = path;
    return result;
}

void WebDAVSynchronizer::prepareReply(QNetworkReply* reply) const
{
    if (m_disableCertificateCheck) {
        connect(reply, &QNetworkReply::sslErrors,
                [=](QList<QSslError> errors) {
            for (auto error : errors) {
                qCWarning(webDAVSynchronizer) << error.errorString();
            }
            reply->ignoreSslErrors(errors);
        });
    }
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            [=](QNetworkReply::NetworkError error) {
        qCWarning(webDAVSynchronizer) << "Network error:" << error;
    });
}


/**
 * @brief Waits for the @p reply to finish.
 *
 * This is a helper method which waits for the network reply to finish, i.e.
 * either the reply's finished() signal is emitted or there was no up- or
 * download progress within a given interval.
 */
void WebDAVSynchronizer::waitForReplyToFinish(QNetworkReply* reply) const
{
    Q_CHECK_PTR(reply);
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(30000);
    auto restartTimer = [&](qint64, qint64) { timer.start(); };
    connect(reply, &QNetworkReply::uploadProgress, restartTimer);
    connect(reply, &QNetworkReply::uploadProgress, restartTimer);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec();
}
