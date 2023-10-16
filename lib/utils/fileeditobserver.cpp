/*
 * Copyright 2023 Martin Hoeher <martin@rpdev.net>
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

#include "fileeditobserver.h"

/**
 * @class FileEditObserver
 * @brief Watch files for changes.
 *
 * This class is used together with the ShareUtils class to edit files. The share utils can be
 * used in two ways: To "send" a file to another app for watching (which is basically a fire and
 * forget action) as well as to edit files. Editing means that the file is opened in another app
 * and we watch for changes to react upon them (e.g. to update the user interface, trigger
 * a sync, and so on).
 *
 * Instances of this class can be passed alongside a request to edit a file. Once we detect
 * changed so the file, the fileChanged() signal of this class will be emitted.
 */

/**
 * @brief Constructor.
 */
FileEditObserver::FileEditObserver(QObject* parent) : QObject { parent } {}

/**
 * @brief The URL of the file which the observer watches.
 */
QUrl FileEditObserver::fileUrl() const
{
    return m_fileUrl;
}

/**
 * @brief Set the URL of the file the observer watches.
 */
void FileEditObserver::setFileUrl(const QUrl& newFileUrl)
{
    if (m_fileUrl == newFileUrl)
        return;
    m_fileUrl = newFileUrl;
    emit fileUrlChanged();
}

/**
 * @brief Notify about file changes.
 *
 * This causes the fileChanged() signal to be emitted.
 */
void FileEditObserver::notify(const QUrl& url)
{
    emit fileChanged(url);
}
