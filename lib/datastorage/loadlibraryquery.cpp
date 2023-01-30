/*
 * Copyright 2023 Martin Hoeher <martin@rpdev.net>
 *
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

#include "loadlibraryquery.h"

#include <QEventLoop>

#include "libraryloader.h"

/**
 * @class LoadLibraryQuery
 * @brief Loads a library directory into the cache.
 *
 * This query can be used to run a LibraryLoader and use it to read the contents of a library folder
 * into the cache. The idea is that the LibraryLoader needs to make use of the Cache - using jobs to
 * run the loader ensures that all jobs finish before the cache is freed.
 */

/**
 * @brief Constructor.
 */
LoadLibraryQuery::LoadLibraryQuery(QObject* parent) : ItemsQuery { parent }
{
    setIsNonDBQuery(true);
}

/**
 * @brief Run the query.
 */
void LoadLibraryQuery::run()
{
    LibraryLoader loader;
    loader.setDirectory(m_directory);
    loader.setLibraryId(m_libraryId);
    loader.setCache(cache());

    QEventLoop loop;
    connect(&loader, &LibraryLoader::scanFinished, &loop, &QEventLoop::quit);
    loader.scan();
    loop.exec();
}

/**
 * @brief The ID of the library to load.
 */
QUuid LoadLibraryQuery::libraryId() const
{
    return m_libraryId;
}

/**
 * @brief Set the ID of the library to load.
 */
void LoadLibraryQuery::setLibraryId(const QUuid& newLibraryId)
{
    if (m_libraryId == newLibraryId)
        return;
    m_libraryId = newLibraryId;
}

/**
 * @brief The directory to load the library from.
 */
QString LoadLibraryQuery::directory() const
{
    return m_directory;
}

/**
 * @brief Set the directory from which to load the library.
 */
void LoadLibraryQuery::setDirectory(const QString& newDirectory)
{
    if (m_directory == newDirectory)
        return;
    m_directory = newDirectory;
}
