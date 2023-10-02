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

#include "genericplatformshareutils.h"

#include <QDesktopServices>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QUrl>
#include <QTimer>
#include <QProcess>

/**
 * @class GenericPlatformShareUtils
 * @brief Generic implementation of sharing facilities.
 */

/**
 * @brief Constructor.
 */
GenericPlatformShareUtils::GenericPlatformShareUtils(QObject* parent)
    : PlatformShareUtils { parent }, m_watcher { new QFileSystemWatcher { this } }
{
}

/**
 * @brief Implementation of PlatformShareUtils::canOpenFolders().
 */
bool GenericPlatformShareUtils::canOpenFolders() const
{
    return true;
}

/**
 * @brief Implementation of PlatformShareUtils::openFile().
 */
void GenericPlatformShareUtils::openFile(const QUrl& url)
{
#ifdef Q_OS_LINUX
    /*
     * This is a workaround for https://bugreports.qt.io/browse/QTBUG-83939. See also
     * https://forum.snapcraft.io/t/xdg-open-or-gvfs-open-qdesktopservices-openurl-file-somelocation-file-txt-wont-open-the-file/16824.
     * We basically try to detect if we run inside a snap and - if so - open file URLs using
     * xdg-open directly.
     */
    // Is this a file URL?
    if (url.isLocalFile()) {
        // Check if we run inside a snap:
        if (qEnvironmentVariableIsSet("SNAP")) {
            // Run "xdg-open" to open the file:
            if (!QProcess::startDetached("xdg-open", { url.toString() })) {
                emit noAppFound(url);
            }
            return;
        }
    }
#endif
    if (!QDesktopServices::openUrl(url)) {
        emit noAppFound(url);
    }
}

/**
 * @brief Implementation of PlatformShareUtils::editFile().
 */
void GenericPlatformShareUtils::editFile(const QUrl& url, FileEditObserver* observer)
{
    PlatformShareUtils::editFile(url, observer); // For observer handling

    // On non-mobile OSes, the file is opened in place and hence, no copying back is required.
    // However, to give the UI the possibility to act on edits, we monitor the file for changes.
    auto filePath = url.toLocalFile();
    m_watcher->addPath(filePath);

    openFile(url);
}

/**
 * @brief Implementation of PlatformShareUtils::openFolder().
 */
void GenericPlatformShareUtils::openFolder(const QString& path)
{
    auto url = QUrl::fromLocalFile(path);
    if (!QDesktopServices::openUrl(url)) {
        emit noFolderExplorerFound(path);
    }
}

/**
 * @brief Implementation of PlatformShareUtils::openUrl().
 */
void GenericPlatformShareUtils::openUrl(const QUrl& url)
{
    if (!QDesktopServices::openUrl(url)) {
        emit noBrowserFound(url);
    }
}

/**
 * @brief Implementation of PlatformShareUtils::unwatchFile().
 */
void GenericPlatformShareUtils::unwatchFile(const QUrl& url)
{
    m_watcher->removePath(url.toLocalFile());
}

void GenericPlatformShareUtils::watchedFileChanged(const QString& path)
{
    // Some apps implement a save by first writing to a new file, then removing the original one
    // and finally moving the new one to the previous path. In this case, check if the path still
    // points to a valid file and watch it again:
    if (!m_watcher->files().contains(path)) {
        if (QFileInfo::exists(path)) {
            m_watcher->addPath(path);
        }
    }

    notifyFileChanged(QUrl::fromLocalFile(path));
}
