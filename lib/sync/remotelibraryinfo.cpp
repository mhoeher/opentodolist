/*
 * Copyright 2022 Martin Hoeher <martin@rpdev.net>
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

#include "remotelibraryinfo.h"

/**
 * @brief Constructor.
 */
RemoteLibraryInfo::RemoteLibraryInfo() : m_name(), m_path(), m_uid()
{
    qRegisterMetaType<RemoteLibraryInfo>();
}

/**
 * @brief The name of the library.
 */
QString RemoteLibraryInfo::name() const
{
    return m_name;
}

/**
 * @brief Set the library name.
 */
void RemoteLibraryInfo::setName(const QString& name)
{
    m_name = name;
}

/**
 * @brief The path (relative to the account's remote directory).
 */
QString RemoteLibraryInfo::path() const
{
    return m_path;
}

/**
 * @brief Set the path of the library.
 */
void RemoteLibraryInfo::setPath(const QString& path)
{
    m_path = path;
}

QUuid RemoteLibraryInfo::uid() const
{
    return m_uid;
}

void RemoteLibraryInfo::setUid(const QUuid& uid)
{
    m_uid = uid;
}

/**
 * @brief Equality operator.
 */
bool RemoteLibraryInfo::operator==(const RemoteLibraryInfo& other) const
{
    return m_name == other.m_name && m_uid == other.m_uid && m_path == other.m_path;
}
