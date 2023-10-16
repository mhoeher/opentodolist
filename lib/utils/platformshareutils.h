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

#ifndef UTILS_PLATFORMSHAREUTILS_H_
#define UTILS_PLATFORMSHAREUTILS_H_

#include <QObject>
#include <QMultiHash>
#include <QPointer>
#include <QUrl>

class FileEditObserver;

class PlatformShareUtils : public QObject
{
    Q_OBJECT
public:
    explicit PlatformShareUtils(QObject* parent = nullptr);

    virtual bool canOpenFolders() const = 0;

public slots:

    virtual void openFile(const QUrl& url) = 0;
    virtual void editFile(const QUrl& url, FileEditObserver* observer = nullptr) = 0;
    virtual void openFolder(const QString& path) = 0;
    virtual void openUrl(const QUrl& url) = 0;

signals:

    void noAppFound(const QUrl& url);
    void noFolderExplorerFound(const QString& path);
    void noBrowserFound(const QUrl& url);

protected slots:

    virtual void unwatchFile(const QUrl& url) = 0;
    void notifyFileChanged(const QUrl& url);

private:
    QMultiHash<QUrl, QPointer<FileEditObserver>> m_watchedFiles;
};

#endif // UTILS_PLATFORMSHAREUTILS_H_
