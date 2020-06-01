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

#include "fileutils.h"

#include <QDir>
#include <QFileInfo>

#include "datamodel/library.h"

bool FileUtils::isSubDirOrFile(const QString& dir, const QString& subDir)
{
    QString parDir = QDir::fromNativeSeparators(dir);
    QString childDir = QDir::fromNativeSeparators(subDir);
    if (!parDir.endsWith("/")) {
        parDir = parDir + "/";
    }
    return childDir.startsWith(parDir) || (dir == subDir);
}

/**
 * @brief Convert a path to make it suitable for persisting.
 *
 * This converts the given @p path and returns a representation which
 * is suitable for being saved to disk. In particular, it:
 *
 * - Strips the default library location from the beginning and replaces it
 *   with a placeholder. This is required on some platforms like iOS, where
 *   the default app locations change between installs/updates.
 */
QString FileUtils::toPersistedPath(const QString& path)
{
    auto libsLocation = Library::defaultLibrariesLocation();
    if (path.startsWith(libsLocation)) {
        return "<APPROOT>" + path.mid(libsLocation.length());
    } else {
        return path;
    }
}

/**
 * @brief Get a path from its persisted form.
 *
 * This is basically the reverse operation of @sa toPersistedPath(). It takes
 * a @p path in its persisted form and returns a valid local path.
 */
QString FileUtils::fromPersistedPath(const QString& path)
{
    auto result = path;
    QString placeholder = "<APPROOT>";
    if (result.startsWith(placeholder)) {
        result = Library::defaultLibrariesLocation() + result.mid(placeholder.length());
    }
    return result;
}
