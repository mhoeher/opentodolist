#include "webdavsynchronizer.h"

WebDAVSynchronizer::WebDAVSynchronizer(QObject* parent) :
    Synchronizer(parent)
{

}

WebDAVSynchronizer::~WebDAVSynchronizer()
{

}

void WebDAVSynchronizer::validate()
{
    // TODO: Implement me
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
