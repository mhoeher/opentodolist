#include "dropboxsynchronizer.h"

DropboxSynchronizer::DropboxSynchronizer(QObject* parent)
    : Synchronizer(parent), m_refreshToken(), m_codeVerifier()
{
}

void DropboxSynchronizer::validate() {}

void DropboxSynchronizer::synchronize() {}

void DropboxSynchronizer::stopSync() {}

void DropboxSynchronizer::findExistingLibraries() {}

QVariantMap DropboxSynchronizer::toMap() const
{
    return QVariantMap {};
}

void DropboxSynchronizer::fromMap(const QVariantMap& map) {}

void DropboxSynchronizer::setAccount(Account* account) {}
