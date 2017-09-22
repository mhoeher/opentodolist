#ifndef SYNCRUNNER_H
#define SYNCRUNNER_H

#include <QObject>
#include <QRunnable>

class SyncJob;


/**
 * @brief Run sync jobs.
 */
class SyncRunner : public QRunnable
{
public:
    SyncRunner(SyncJob *job);
    virtual ~SyncRunner();

    // QRunnable interface
    void run() override;

private:

    SyncJob *m_job;
};

#endif // SYNCRUNNER_H
