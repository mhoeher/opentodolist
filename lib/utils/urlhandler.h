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

#ifndef UTILS_URLHANDLER_H_
#define UTILS_URLHANDLER_H_

#include <QObject>
#include <QUrl>
#include <QPointer>

class Item;
class Library;

class UrlHandler : public QObject
{
    Q_OBJECT
public:
    static const char* Scheme;

    static const char* DropboxAuthHost;
    static const char* ItemHost;
    static const char* LibraryHost;

    static const char* DropboxAuthUrl;

    explicit UrlHandler(QObject* parent = nullptr);

    void initialize();

    static UrlHandler* instance();

    static QUrl createDeepLink(Item& item);
    static QUrl createDeepLink(Library& library);

signals:

    /**
     * @brief A generic link has been received.
     */
    void urlReceived(const QUrl& url);

    /**
     * @brief A Dropbox OAuth callback link has been received.
     */
    void dropboxAuthReceived(const QUrl& url);

    /**
     * @brief A deep link to the item with the @p itemUid has been received.
     */
    void itemLinkReceived(const QUrl& url, QUuid& itemUid);

    /**
     * @brief A deep link to the library with the @p libraryUid has been received.
     */
    void libraryLinkReceived(const QUrl& url, QUuid& libraryUid);

private:
    static QPointer<UrlHandler> s_instance;

    static QUuid uidFromUrl(const QUrl& url);

private slots:

    void handleUrl(const QUrl& url);

    void setupAndroidAppLinkHandler();
};

#endif // UTILS_URLHANDLER_H_
