#include "webdavsynchronizer.h"

#include <QDir>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>


Q_LOGGING_CATEGORY(webDAVSynchronizer,
                   "net.rpdev.opentodolist.WebDAVSynchronizer",
                   QtDebugMsg)


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
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            [=](QNetworkReply::NetworkError error) {
        qCWarning(webDAVSynchronizer) << "Error during validation:" << error;
    });
    connect(reply, &QNetworkReply::sslErrors,
            [=](QList<QSslError> errors) {
        for (auto error : errors) {
            qCWarning(webDAVSynchronizer) << "SSL Error during validation:"
                                          << error.errorString();
        }
    });
    connect(reply, &QNetworkReply::finished, [=]() {
        auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (code == 207) { // 207 MULTI-STATUS
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

WebDAVSynchronizer::EntryList WebDAVSynchronizer::entryList(const QString& directory)
{
    // TODO: Implement me
    Q_UNUSED(directory);
    EntryList result;
    return result;
}

bool WebDAVSynchronizer::get(const QString& filename, const QString& localFileName)
{
    // TODO: Implement me
    Q_UNUSED(filename);
    Q_UNUSED(localFileName);
    return false;
}

bool WebDAVSynchronizer::put(const QString& localFileName, const QString& filename)
{
    // TODO: Implement me
    Q_UNUSED(localFileName);
    Q_UNUSED(filename);
    return false;
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
                             "<a:displayname/>"
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
    auto reply = m_networkAccessManager->sendCustomRequest(
                request, "PROPFIND", requestData);
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
    auto path = element.firstChildElement("href").text();
    path = QDir(baseDir).relativeFilePath(path);
    Entry result;
    result.type = File;
    result.name = path;
    return result;
}
