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

#ifndef UTILS_GENERICPLATFORMSHAREUTILS_H_
#define UTILS_GENERICPLATFORMSHAREUTILS_H_

#include <QMultiHash>

#include "platformshareutils.h"

class QFileSystemWatcher;

class GenericPlatformShareUtils : public PlatformShareUtils
{
    Q_OBJECT
public:
    explicit GenericPlatformShareUtils(QObject* parent = nullptr);

private:
    QFileSystemWatcher* m_watcher;

private slots:

    void watchedFileChanged(const QString& path);

    // PlatformShareUtils interface
public:
    bool canOpenFolders() const override;

public slots:
    void openFile(const QUrl& url) override;
    void editFile(const QUrl& url, FileEditObserver* observer = nullptr) override;
    void openFolder(const QString& path) override;
    void openUrl(const QUrl& url) override;

private slots:
    void unwatchFile(const QUrl& url) override;
};

#endif // UTILS_GENERICPLATFORMSHAREUTILS_H_
