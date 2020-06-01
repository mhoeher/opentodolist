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

#include "directorywatcher.h"

#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>

/**
 * @brief Constructor.
 */
DirectoryWatcher::DirectoryWatcher(QObject* parent)
    : QObject(parent), m_thread(new QThread(this)), m_worker(new DirectoryWatcherWorker())
{
    m_thread->setObjectName("DirectoryWatcher");
    m_thread->start();
    m_worker->moveToThread(m_thread);
    connect(m_worker, &DirectoryWatcherWorker::directoryChanged, this,
            &DirectoryWatcher::directoryChanged);
}

/**
 * @brief Destructor.
 */
DirectoryWatcher::~DirectoryWatcher()
{
    m_worker->deleteLater();
    m_thread->quit();
    if (!m_thread->wait(5000)) {
        m_thread->terminate();
        m_thread->wait(5000);
    }
}

/**
 * @brief Set the directory to watch for changes.
 */
void DirectoryWatcher::setDirectory(const QString& directory)
{
    QMetaObject::invokeMethod(m_worker, "setDirectory", Qt::QueuedConnection,
                              Q_ARG(const QString&, directory));
}

DirectoryWatcherWorker::DirectoryWatcherWorker() : QObject(), m_watcher(nullptr), m_directory() {}

DirectoryWatcherWorker::~DirectoryWatcherWorker() {}

void DirectoryWatcherWorker::setDirectory(const QString& directory)
{
    if (m_directory != directory) {
        m_directory = directory;

        // Create watcher on-demand when we first set a directory.
        if (m_watcher == nullptr) {
            m_watcher = new QFileSystemWatcher(this);
            connect(m_watcher, &QFileSystemWatcher::directoryChanged, [=](const QString& dir) {
                emit directoryChanged();
                if (!m_directory.isEmpty()) {
                    this->watchDir(dir);
                }
            });
            connect(m_watcher, &QFileSystemWatcher::fileChanged,
                    [=](const QString&) { emit directoryChanged(); });
        }

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
