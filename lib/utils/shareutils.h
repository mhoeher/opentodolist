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

#ifndef UTILS_SHAREUTILS_H_
#define UTILS_SHAREUTILS_H_

#include <QObject>
#include <QScopedPointer>

class PlatformShareUtils;
class FileEditObserver;
class Item;
class Library;

class ShareUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool canOpenFolders READ canOpenFolders CONSTANT)
public:
    explicit ShareUtils(QObject* parent = nullptr);
    virtual ~ShareUtils();

    bool canOpenFolders() const;

    Q_INVOKABLE QUrl createDeepLink(Item* item);
    Q_INVOKABLE QUrl createDeepLink(Library* library);

public slots:

    void openLink(const QString& url);
    void openLink(const QUrl& url);
    void openFile(const QUrl& url);
    void openFile(const QString& path);
    void editFile(const QUrl& url, FileEditObserver* observer = nullptr);
    void editFile(const QString& path, FileEditObserver* observer = nullptr);
    void openFolder(const QString& path);

signals:

    void noAppFound(const QUrl& url);
    void noFolderExplorerFound(const QString& path);
    void noBrowserFound(const QUrl& url);

private:
    QScopedPointer<PlatformShareUtils> m_platformShareUtils;
};

#endif // UTILS_SHAREUTILS_H_
