#include "syncjob.h"

#include "synchronizer.h"

/**
 * @brief Create a new sync job.
 *
 * This creates a new sync job for the @p library which
 * stores it's data in the @p libraryDirectory. The @p secret is
 * the passphrase or key required by the library's synchronizer
 * to talk to the backend server (if required).
 *
 * @note The library is only used as a handle. None of it's methods
 * are called, as it might get deleted while the sync is running
 * in the background.
 */
SyncJob::SyncJob(Library *library,
                 const QString &libraryDirectory,
                 const QString &secret,
                 QObject *parent) : QObject(parent),
    m_library(library),
    m_libraryDirectory(libraryDirectory),
    m_secret(secret)
{

}


/**
 * @brief Run the synchronization.
 */
void SyncJob::execute()
{
    if (!m_libraryDirectory.isEmpty()) {
        QScopedPointer<Synchronizer> sync(
                    Synchronizer::fromDirectory(m_libraryDirectory));
        if (sync) {
            auto key = sync->secretsKey();
            if (!key.isEmpty()) {
                sync->setSecret(m_secret);
            }
            connect(this, &SyncJob::stopRequested,
                    sync.data(), &Synchronizer::stopSync,
                    Qt::QueuedConnection);
            sync->synchronize();
        }
    }
    emit syncFinished(m_library);
}

void SyncJob::stop()
{
    emit stopRequested();
}
