#include "webdavsynchronizer.h"

#include <QBuffer>
#include <QDir>
#include <QDomDocument>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTemporaryFile>
#include <QTimer>


Q_LOGGING_CATEGORY(webDAVSynchronizer,
                   "net.rpdev.opentodolist.WebDAVSynchronizer",
                   QtDebugMsg)

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

bool operator ==(int lhs, HTTPStatusCode rhs) {
    return lhs == static_cast<int>(rhs);
}

bool operator ==(HTTPStatusCode lhs, int rhs) {
    return static_cast<int>(lhs) == rhs;
}




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
        if (code == HTTPStatusCode::WebDAVMultiStatus) {
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
        auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (code == HTTPStatusCode::WebDAVMultiStatus) {
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
    QTemporaryFile* file = new QTemporaryFile();
    auto result = false;
    if (file->open()) {
        QNetworkRequest request;
        auto url = QUrl(baseUrl().toString() +
                        QDir::cleanPath("/" + remoteDirectory() + "/" + filename));
        url.setUserName(username());
        url.setPassword(password());
        request.setUrl(url);
        auto reply = m_networkAccessManager->get(request);
        file->setParent(reply);
        connect(reply, &QNetworkReply::readyRead, [=]() {
            file->write(reply->readAll());
        });
        waitForReplyToFinish(reply);
        file->write(reply->readAll());
        auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (code == HTTPStatusCode::OK) {
            QFile targetFile(directory() + "/" + filename);
            if (targetFile.open(QIODevice::WriteOnly)) {
                file->seek(0);
                while (!file->atEnd()) {
                    targetFile.write(file->read(1024*1024));
                }
                targetFile.close();
                result = true;
            } else {
                qCWarning(webDAVSynchronizer) << "Failed to open destination"
                                              << "file for writing:"
                                              << targetFile.errorString();
            }
        } else {
            qCWarning(webDAVSynchronizer) << "Upload failed with code" << code;
        }
    } else {
        qCWarning(webDAVSynchronizer) << "Failed to open temporary file for"
                                      << "downloading:"
                                      << file->errorString();
    }
    return result;
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
        if (code == HTTPStatusCode::OK || code == HTTPStatusCode::Created ||
                code == HTTPStatusCode::NoContent) {
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
        result = code.toInt() == HTTPStatusCode::Created;
    });
    waitForReplyToFinish(reply);
    return result;
}

bool WebDAVSynchronizer::deleteEntry(const QString& filename)
{
    auto result = false;
    QNetworkRequest request;
    auto url = QUrl(baseUrl().toString() +
                    QDir::cleanPath("/" + remoteDirectory() + "/" + filename));
    url.setUserName(username());
    url.setPassword(password());
    request.setUrl(url);
    auto reply = m_networkAccessManager->deleteResource(request);
    waitForReplyToFinish(reply);
    auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (code == HTTPStatusCode::OK || code == HTTPStatusCode::NoContent) {
        result = true;
    } else {
        qCWarning(webDAVSynchronizer) << "Deleting entry failed with code"
                                      << code;
    }
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
        qCWarning(webDAVSynchronizer) << "Received invalid WebDAV response from"
                                         "server starting with element"
                                      << rootTagName;
    }
    return result;
}

WebDAVSynchronizer::Entry WebDAVSynchronizer::parseResponseEntry(
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


    QString path = QByteArray::fromPercentEncoding(
                element.firstChildElement("d:href").text().toUtf8());

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
