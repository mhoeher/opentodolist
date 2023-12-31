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

#include "shareutils.h"

#include <QDesktopServices>
#include <QUrl>

#include "utils/urlhandler.h"

#if defined(Q_OS_IOS)
#    include "iosplatformshareutils.h"
#elif defined(Q_OS_ANDROID)
#    include "androidplatformshareutils.h"
#else
#    include "genericplatformshareutils.h"
#endif

/**
 * @class ShareUtils
 * @brief Share files with other apps.
 *
 * This class can be used to share files and other resources with apps on the device.
 * It uses platform specific functionality to do so - in particular on mobile systems, sharing needs
 * special care compared to desktop systems (where files typically can be opened "in place").
 */

/**
 * @brief Constructor.
 */
ShareUtils::ShareUtils(QObject* parent) : QObject { parent }
{
#if defined(Q_OS_IOS)
    m_platformShareUtils.reset(new IOSPlatformShareUtils);
#elif defined(Q_OS_ANDROID)
    m_platformShareUtils.reset(new AndroidPlatformShareUtils);
#else
    m_platformShareUtils.reset(new GenericPlatformShareUtils);
#endif

    connect(m_platformShareUtils.data(), &PlatformShareUtils::noAppFound, this,
            &ShareUtils::noAppFound);
    connect(m_platformShareUtils.data(), &PlatformShareUtils::noFolderExplorerFound, this,
            &ShareUtils::noFolderExplorerFound);
    connect(m_platformShareUtils.data(), &PlatformShareUtils::noBrowserFound, this,
            &ShareUtils::noBrowserFound);
}

/**
 * @brief Destructor.
 */
ShareUtils::~ShareUtils() {}

/**
 * @brief Indicates if folders can be opened.
 *
 * On supported platforms, this returns true and indicates that via a call to
 * openFolder() the given folder path can be browsed in a platform specific way.
 *
 * Otherwise, calls to openFolder() must not be made.
 */
bool ShareUtils::canOpenFolders() const
{
    return m_platformShareUtils->canOpenFolders();
}

/**
 * @brief Create a deep link to the given @p item.
 */
QUrl ShareUtils::createDeepLink(Item* item)
{
    QUrl result;
    if (item != nullptr) {
        result = UrlHandler::createDeepLink(*item);
    }
    return result;
}

/**
 * @brief Create a deep link to the given @p library.
 */
QUrl ShareUtils::createDeepLink(Library* library)
{
    QUrl result;
    if (library != nullptr) {
        result = UrlHandler::createDeepLink(*library);
    }
    return result;
}

/**
 * @brief Open a link.
 *
 * This is an overloaded variant which takes the @p url to be opened as a string.
 */
void ShareUtils::openLink(const QString& url)
{
    openLink(QUrl(url));
}

/**
 * @brief Open a link.
 *
 * This opens the link given by the @p url.
 *
 * If no app can be found to open the link, the noBrowserFound() signal will be emitted with the
 * url.
 */
void ShareUtils::openLink(const QUrl& url)
{
    m_platformShareUtils->openUrl(url);
}

/**
 * @brief Open a file in another app.
 *
 * This will open the file pointed to by the given @p url in an external app.
 *
 * This operation usually happens asynchronously. Don't rely on the file already being open when
 * this function returns.
 *
 * In case no suitable app was found to open the file, the noAppFound() signal will be emitted.
 */
void ShareUtils::openFile(const QUrl& url)
{
    m_platformShareUtils->openFile(url);
}

/**
 * @brief Share a file to another app.
 *
 * This overloaded version of the method allows specifying the location of the file to be
 * shared as a file @p path.
 */
void ShareUtils::openFile(const QString& path)
{
    openFile(QUrl::fromLocalFile(path));
}

/**
 * @brief Edit a file.
 *
 * This opens the file pointed to by the file @p url in a suitable app. If the file
 * was changed, the observer will be notified. If no observer is given, this is basically
 * identical to the openFile() method.
 */
void ShareUtils::editFile(const QUrl& url, FileEditObserver* observer)
{
    m_platformShareUtils->editFile(url, observer);
}

/**
 * @brief Edit a file.
 *
 * This is an overloaded version of the method which takes a local file path instead of a URL.
 */
void ShareUtils::editFile(const QString& path, FileEditObserver* observer)
{
    editFile(QUrl::fromLocalFile(path), observer);
}

/**
 * @brief Open a folder.
 *
 * This opens the folder pointed to by the @p path in a platform specific way.
 *
 * This method shall only be called if canOpenFolders() returns true.
 *
 * In case opening the failed fails, the noFolderExplorerFound() signal is emitted with
 * the path parameters.
 */
void ShareUtils::openFolder(const QString& path)
{
    if (canOpenFolders()) {
        m_platformShareUtils->openFolder(path);
    }
}

/**
 * @fn ShareUtils::noAppFound()
 * @brief No app has been found to open a file.
 *
 * This signal is emitted when there was no suitable app found to open the file pointed to by the
 * @p url.
 */

/**
 * @fn ShareUtils::noFolderExplorerFound()
 * @brief No app found to browse folders.
 *
 * This signal is emitted when there was no suitable app found to browse a folder.
 * The @p path is the same as passed in to the openFolder() method.
 */

/**
 * @fn ShareUtils::noBrowserFound()
 * @brief No suitable app found to open a link.
 *
 * This signal is emitted when opening a link failed. The @p url is the URL for which we didn't
 * find an app.
 */
