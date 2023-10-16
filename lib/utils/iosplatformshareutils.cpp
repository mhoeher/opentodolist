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

#include "iosplatformshareutils.h"

IOSPlatformShareUtils::IOSPlatformShareUtils(QObject* parent) : PlatformShareUtils { parent } {}

void IOSPlatformShareUtils::previewDone(const QUrl& url)
{
    // TODO: Should we check if the file really changed here?
    notifyFileChanged(url);
}

bool IOSPlatformShareUtils::canOpenFolders() const
{
    return false;
}

/**
 * @brief Implementation of PlatformShareUtils::openFolder().
 *
 * This operation is not supported on iOS.
 */
void IOSPlatformShareUtils::openFolder(const QString& path)
{
    Q_UNUSED(path);
}
