/*
 * Copyright 2022 Martin Hoeher <martin@rpdev.net>
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

#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_

#include <QString>
#include <QUrl>
#include <QVector>
#include <QMap>
#include <QTest>

#include <tuple>

#include "sync/webdavaccount.h"

/**
 * @brief Contains information about a WebDAV server to test afainst.
 */
struct WebDAVServerInfo
{
    WebDAVAccount::Type type;
    QUrl url;
    QString name;
};

static const QMap<QString, std::tuple<const char*, WebDAVAccount::Type>> WebDAVSchemeLookupMap {
    { "http", std::make_tuple("http", WebDAVAccount::WebDAV) },
    { "https", std::make_tuple("https", WebDAVAccount::WebDAV) },
    { "generic", std::make_tuple("http", WebDAVAccount::WebDAV) },
    { "generics", std::make_tuple("httpss", WebDAVAccount::WebDAV) },
    { "nextcloud", std::make_tuple("http", WebDAVAccount::NextCloud) },
    { "nextclouds", std::make_tuple("https", WebDAVAccount::NextCloud) },
    { "owncloud", std::make_tuple("http", WebDAVAccount::OwnCloud) },
    { "ownclouds", std::make_tuple("https", WebDAVAccount::OwnCloud) },
};

/**
 * @brief Return a list of WebDAV test servers.
 *
 * This function parses the environment variable OPENTODOLIST_WEBDAV_TEST_SERVERS and
 * returns a list of configuration objects that hold the information needed to interact with
 * the server. The OPENTODOLIST_WEBDAV_TEST_SERVERS variable is a semicolon separated list
 * of URLs of WebDAV servers to test against. URLs are supposed to contain the username and
 * password needed to authenticate against the server. In addition, the following special
 * rules apply for the scheme:
 *
 * - 'http://' and 'generic://' will be mapped to a generic WebDAV server using HTTP.
 * - 'https://' and 'generics://' will be mapped to a generic WebDAV server using HTTPS.
 * - 'nextcloud://' will be mapped to a NextCloud server using HTTP.
 * - 'nextclouds://' will be mapped to a NextCloud server using HTTPS.
 * - 'owncloud://' will be mapped to an ownCloud server using HTTP.
 * - 'ownclouds://' will be mapped to an ownCloud server using HTTPS.
 */
inline QVector<WebDAVServerInfo> readConfiguredWebDAVTestServers()
{
    const auto values = qgetenv("OPENTODOLIST_WEBDAV_TEST_SERVERS").split(';');
    QVector<WebDAVServerInfo> result;
    for (const auto& value : values) {
        QUrl url(QString::fromUtf8(value));
        if (url.isValid()) {
            const auto scheme = url.scheme();
            const auto entry = WebDAVSchemeLookupMap.find(scheme);
            if (entry != WebDAVSchemeLookupMap.end()) {
                WebDAVServerInfo info;
                info.name = url.toString();
                info.type = std::get<1>(entry.value());
                url.setScheme(std::get<0>(entry.value()));
                info.url = url;
                result << info;
            }
        }
    }
    return result;
}

template<WebDAVAccount::Type ServerType>
inline void addWebDAVServerTestUrls()
{
    QTest::addColumn<QUrl>("url");

    const auto servers = readConfiguredWebDAVTestServers();
    int count = 0;
    for (const auto& server : servers) {
        if (server.type == ServerType) {
            QTest::newRow(server.url.toString().toUtf8()) << server.url;
            ++count;
        }
    }
    if (count == 0) {
        QSKIP("No suitable test servers defined - skipping test");
    }
}

#endif // TEST_UTILS_H_
