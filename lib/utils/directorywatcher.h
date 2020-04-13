#ifndef DIRECTORYWATCHER_H
#define DIRECTORYWATCHER_H

#include <QObject>
#include <QThread>

class QFileSystemWatcher;
class DirectoryWatcherWorker;

/**
 * @brief A helper class used to watch a directory for any changes.
 *
 * This is a helper class which is used to monitor a single directory (non-recursively) for
 * any changes, including:
 *
 * - adding files
 * - removing files
 * - any contained files changed
 *
 * For any recognized change, the directoryChanged() signal is emitted.
 */
class DirectoryWatcher : public QObject
{
    Q_OBJECT
public:
    explicit DirectoryWatcher(QObject *parent = nullptr);
    virtual ~DirectoryWatcher();

signals:

    /**
     * @brief Something in the directory has changed.
     */
    void directoryChanged();

public slots:

    void setDirectory(const QString &directory);

private:
    QThread *m_thread;
    DirectoryWatcherWorker *m_worker;
};

class DirectoryWatcherWorker : public QObject
{
    Q_OBJECT

    friend class DirectoryWatcher;

    QFileSystemWatcher *m_watcher;
    QString m_directory;

    explicit DirectoryWatcherWorker();
    virtual ~DirectoryWatcherWorker();

signals:

    void directoryChanged();

private slots:

    void setDirectory(const QString &directory);
    void watchDir(const QString &directory);
};

#endif // DIRECTORYWATCHER_H
