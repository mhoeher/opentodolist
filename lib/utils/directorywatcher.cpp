#include "directorywatcher.h"

#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>

/**
 * @brief Constructor.
 */
DirectoryWatcher::DirectoryWatcher(QObject *parent) : QObject(parent),
    m_thread(new QThread(this)),
    m_worker(new DirectoryWatcherWorker())
{
    m_thread->setObjectName("DirectoryWatcher");
    m_thread->start();
    m_worker->moveToThread(m_thread);
    connect(m_worker, &DirectoryWatcherWorker::directoryChanged,
            this, &DirectoryWatcher::directoryChanged);
}

/**
 * @brief Destructor.
 */
DirectoryWatcher::~DirectoryWatcher()
{
    m_thread->quit();
    if (!m_thread->wait(5000)) {
        m_thread->terminate();
        m_thread->wait(5000);
    }
    delete m_worker;
}

/**
 * @brief Set the directory to watch for changes.
 */
void DirectoryWatcher::setDirectory(const QString& directory)
{
    QMetaObject::invokeMethod(m_worker, "setDirectory", Qt::QueuedConnection,
                              Q_ARG(const QString&, directory));
}

DirectoryWatcherWorker::DirectoryWatcherWorker():
    QObject(),
    m_watcher(new QFileSystemWatcher(this)),
    m_directory()
{
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, [=](const QString& dir) {
        emit directoryChanged();
        if (!m_directory.isEmpty()) {
            this->watchDir(dir);
        }
    });
    connect(m_watcher, &QFileSystemWatcher::fileChanged, [=](const QString&) {
        emit directoryChanged();
    });
}

DirectoryWatcherWorker::~DirectoryWatcherWorker()
{
}

void DirectoryWatcherWorker::setDirectory(const QString& directory)
{
    if (m_directory != directory) {
        m_directory = directory;
        auto items = m_watcher->files() + m_watcher->directories();
        if (!items.isEmpty()) {
            m_watcher->removePaths(items);
        }
        if (!m_directory.isEmpty()) {
            watchDir(directory);
        }
    }
}

/**
 * @brief Watch the directory recursively.
 */
void DirectoryWatcherWorker::watchDir(const QString& directory)
{
    if (directory != "") {
        QDir dir(directory);
        if (dir.exists()) {
            m_watcher->addPath(directory);
            for (auto entry : dir.entryList(QDir::Files)) {
                m_watcher->addPath(dir.absoluteFilePath(entry));
            }
            for (auto entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                watchDir(dir.absoluteFilePath(entry));
            }
        }
    }
}
