#include "dropboxsynchronizer.h"

#include "dropboxaccount.h"

DropboxSynchronizer::DropboxSynchronizer(QObject* parent) : Synchronizer(parent), m_accessToken() {}

void DropboxSynchronizer::synchronize() {}

void DropboxSynchronizer::stopSync() {}

void DropboxSynchronizer::findExistingLibraries() {}

QVariantMap DropboxSynchronizer::toMap() const
{
    return QVariantMap {};
}

void DropboxSynchronizer::fromMap(const QVariantMap& map) {}

void DropboxSynchronizer::setAccount(Account* account)
{
    auto dropboxAccount = qobject_cast<DropboxAccount*>(account);
    q_check_ptr(dropboxAccount);

    setAccessToken(dropboxAccount->accessToken());
}

const QString& DropboxSynchronizer::accessToken() const
{
    return m_accessToken;
}

void DropboxSynchronizer::setAccessToken(const QString& newAccessToken)
{
    if (m_accessToken == newAccessToken)
        return;
    m_accessToken = newAccessToken;
    emit accessTokenChanged();
}
