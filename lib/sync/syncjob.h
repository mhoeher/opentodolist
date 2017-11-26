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
    explicit SyncJob(Library *library,
                     const QString &libraryDirectory,
                     const QString &secret,
                     QObject *parent = nullptr);

    void execute();

signals:

    void syncFinished(Library *library);

    void stopRequested();
    void syncError(const QString& error);

public slots:

    void stop();

private:

    Library *m_library;
    QString  m_libraryDirectory;
    QString  m_secret;
};

#endif // SYNCJOB_H
