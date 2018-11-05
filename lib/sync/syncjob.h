#ifndef SYNCJOB_H
#define SYNCJOB_H

#include <QObject>

class Library;

/**
 * @brief A synchronizing job.
 */
class SyncJob : public QObject
{
    Q_OBJECT
public:
    explicit SyncJob(const QString &libraryDirectory,
                     const QString &secret,
                     QObject *parent = nullptr);

    void execute();

signals:

    void syncFinished(QString libraryDirectory);

    void stopRequested();
    void syncError(QString libraryDirectory, const QString& error);

public slots:

    void stop();

private:

    QString  m_libraryDirectory;
    QString  m_secret;

private slots:

    void onSyncError(QString error);

};

#endif // SYNCJOB_H
