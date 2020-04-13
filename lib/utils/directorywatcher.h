/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_DIRECTORYWATCHER_H_
#define UTILS_DIRECTORYWATCHER_H_

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

    DirectoryWatcherWorker();
    virtual ~DirectoryWatcherWorker();

signals:

    void directoryChanged();

private slots:

    void setDirectory(const QString &directory);
    void watchDir(const QString &directory);
};

#endif // UTILS_DIRECTORYWATCHER_H_
