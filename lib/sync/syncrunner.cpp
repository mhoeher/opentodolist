#include "syncrunner.h"

#include "syncjob.h"

/**
 * @brief Create a sync runner.
 *
 * Creates a new sync runner. The runner will execute the given @p job.
 * The runner takes ownership of the job.
 */
SyncRunner::SyncRunner(SyncJob *job) : QRunnable(), m_job(job)
{
    Q_CHECK_PTR(job);
}

/**
 * @brief Destructor.
 */
SyncRunner::~SyncRunner()
{
    delete m_job;
}

/**
 * @brief Run the sync job.
 */
void SyncRunner::run()
{
    m_job->execute();
}
