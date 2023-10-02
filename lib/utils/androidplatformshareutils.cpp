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

#include "androidplatformshareutils.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QJniObject>
#include <QJniEnvironment>
#include <QStandardPaths>
#include <QDir>
#include <QLoggingCategory>
#include <QFile>
#include <QFileInfo>

static Q_LOGGING_CATEGORY(log, "OpenTodoList.AndroidPlatformShareUtils", QtDebugMsg);

AndroidPlatformShareUtils::AndroidPlatformShareUtils(QObject* parent)
    : PlatformShareUtils { parent }
{
}

/**
 * @brief Implementation of PlatformShareUtils::canOpenFolders().
 */
bool AndroidPlatformShareUtils::canOpenFolders() const
{
    return false;
}

/**
 * @brief Implementation of PlatformShareUtils::openFile().
 */
void AndroidPlatformShareUtils::openFile(const QUrl& url)
{
    // First, move the file into the "shared-files" subfolder of the app's private files. This
    // folder is provided via FileProvider to other apps:
    auto sharedFilesPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
            + "/shared-files";
    auto tmpPath = sharedFilesPath + "/tmp";

    QDir tmpDir(tmpPath);
    if (tmpDir.exists()) {
        // Try to clean up previous share
        if (!tmpDir.removeRecursively()) {
            qCWarning(log) << "Failed to remove temporary path" << tmpPath;
        }
    }

    if (!QDir(tmpPath).mkpath(".")) {
        qCWarning(log) << "Failed to create temporary path" << tmpPath << "for sharing file";
        emit noAppFound(url);
        return;
    }

    auto filePath = url.toLocalFile();
    auto targetPath = tmpPath + "/" + QFileInfo(filePath).fileName();
    if (!QFile::copy(filePath, targetPath)) { // NOLINT
        // TODO: Do this in the background to not block GUI?
        qCWarning(log) << "Failed to copy" << filePath << "to" << targetPath;
        emit noAppFound(url);
        return;
    }

    auto future = QNativeInterface::QAndroidApplication::runOnAndroidMainThread([=]() {
        auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
        if (activity.isValid()) {
            auto path = QJniObject::fromString(targetPath);
            auto ok = activity.callMethod<jboolean>("openFile", "(Ljava/lang/String;)Z",
                                                    path.object<jstring>());
            return ok != JNI_FALSE;
        }
        return false;
    });
    future.waitForFinished();
    if (!future.result().toBool()) {
        emit noAppFound(url);
    }
}

/**
 * @brief Implementation of PlatformShareUtils::editFile().
 */
void AndroidPlatformShareUtils::editFile(const QUrl& url, FileEditObserver* observer)
{
    // TODO: Implement this
    Q_UNUSED(observer);
    emit noAppFound(url);
}

/**
 * @brief Implementation of PlatformShareUtils::openFolder().
 */
void AndroidPlatformShareUtils::openFolder(const QString& path)
{
    // Not supported on Android.
    Q_UNUSED(path);
}

/**
 * @brief Implementation of PlatformShareUtils::openUrl().
 */
void AndroidPlatformShareUtils::openUrl(const QUrl& url)
{
    if (!QDesktopServices::openUrl(url)) {
        emit noBrowserFound(url);
    }
}

/**
 * @brief Implementation of PlatformShareUtils::unwatchFile().
 */
void AndroidPlatformShareUtils::unwatchFile(const QUrl& url)
{
    // TODO: Not implemented yet - to be done once editing is implemented for Android
    Q_UNUSED(url);
}
