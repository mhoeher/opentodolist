#ifndef SYNCJOB_H
#define SYNCJOB_H

#include <QObject>
#include <QSharedPointer>

class Library;
class Account;

/**
 * @brief A synchronizing job.
 */
class SyncJob : public QObject
{
    Q_OBJECT
public:
    explicit SyncJob(const QString &libraryDirectory, QSharedPointer<Account> account,
                     QObject *parent = nullptr);

    void execute();

signals:

    void syncFinished(QString libraryDirectory);

    void stopRequested();
    void syncError(QString libraryDirectory, const QString &error);

public slots:

    void stop();

private:
    QString m_libraryDirectory;
    QSharedPointer<Account> m_account;

private slots:

    void onSyncError(const QString &error);
};

#endif // SYNCJOB_H
