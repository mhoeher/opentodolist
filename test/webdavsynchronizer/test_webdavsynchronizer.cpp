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

#include <QObject>
#include <QObjectList>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <QUrl>
#include <QUuid>
#include <QSet>

#include "sync/webdavsynchronizer.h"
#include "../test-utils.h"

#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/image.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/task.h"
#include "datastorage/cache.h"
#include "datastorage/loadlibraryquery.h"
#include "models/itemsmodel.h"
#include "models/librariesmodel.h"

class WebDAVSynchronizerTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void remoteDirectory();
    void disableCertificateCheck();
    void username();
    void password();
    void synchronize();
    void synchronize_data();
    void cleanup() {}
    void cleanupTestCase() {}

private:
    void createDavClients();
    void echoToFile(const QString& text, const QString& filename);
    QByteArray catFile(const QString& filename);
};

void WebDAVSynchronizerTest::remoteDirectory()
{
    WebDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::remoteDirectoryChanged);
    QCOMPARE(sync.remoteDirectory(), QString(""));
    sync.setRemoteDirectory("/foo/");
    QCOMPARE(sync.remoteDirectory(), QString("/foo/"));
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::disableCertificateCheck()
{
    WebDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::disableCertificateCheckChanged);
    QVERIFY(!sync.disableCertificateCheck());
    sync.setDisableCertificateCheck(true);
    QVERIFY(sync.disableCertificateCheck());
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::username()
{
    WebDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::usernameChanged);
    QCOMPARE(sync.username(), QString(""));
    sync.setUsername("sample.user");
    QCOMPARE(sync.username(), QString("sample.user"));
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::password()
{
    WebDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::passwordChanged);
    QCOMPARE(sync.password(), QString(""));
    sync.setPassword("fooBarBas123!");
    QCOMPARE(sync.password(), QString("fooBarBas123!"));
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::synchronize()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);

    QTemporaryDir dir1;
    QTemporaryDir dir2;

    Library library(dir1.path());
    library.save();
    Note note(library.newItemLocation());
    note.setLibraryId(library.uid());
    note.save();
    Image image(library.newItemLocation());
    image.setLibraryId(library.uid());
    image.save();
    TodoList todoList(library.newItemLocation());
    todoList.setLibraryId(library.uid());
    todoList.save();
    Todo todo(library.newItemLocation());
    todo.setTodoListUid(todoList.uid());
    todo.save();
    Task task(library.newItemLocation());
    task.setTodoUid(todo.uid());
    task.save();

    auto dirName = QUuid::createUuid().toString() + "-" + __func__;
    davClient->setDirectory(dir1.path());
    davClient->setRemoteDirectory(dirName);
    QList<bool> syncState;
    QVERIFY(!davClient->synchronizing());
    connect(davClient, &WebDAVSynchronizer::synchronizingChanged, this,
            [&]() { syncState << davClient->synchronizing(); });
    davClient->synchronize();
    QVERIFY(!davClient->synchronizing());
    QCOMPARE(syncState.count(), 2);
    QVERIFY(syncState[0]);
    QVERIFY(!syncState[1]);

    davClient->setDirectory(dir2.path());
    davClient->synchronize();

    {
        Library library2(dir2.path());
        QTemporaryDir tmpDir;
        Cache cache;
        cache.setCacheDirectory(tmpDir.path());
        QVERIFY(cache.open());
        auto q = new LoadLibraryQuery;
        q->setLibraryId(library.uid());
        q->setDirectory(dir2.path());
        QSignalSpy spy(q, &LoadLibraryQuery::finished);
        cache.run(q);
        QVERIFY(spy.wait());

        {
            LibrariesModel libraries;
            libraries.setCache(&cache);
            QSignalSpy rowsInserted(&libraries, &LibrariesModel::rowsInserted);
            QVERIFY(rowsInserted.wait());
            QCOMPARE(libraries.rowCount(), 1);
        }

        {
            ItemsModel items;
            items.setCache(&cache);
            items.setParentItem(library.uid());
            QSignalSpy rowsInserted(&items, &ItemsModel::rowsInserted);
            QVERIFY(rowsInserted.wait());
            QCOMPARE(items.rowCount(), 3);
        }

        {
            ItemsModel items;
            items.setCache(&cache);
            items.setParentItem(todoList.uid());
            QSignalSpy rowsInserted(&items, &ItemsModel::rowsInserted);
            QVERIFY(rowsInserted.wait());
            QCOMPARE(items.rowCount(), 1);
        }

        {
            ItemsModel items;
            items.setCache(&cache);
            items.setParentItem(todo.uid());
            QSignalSpy rowsInserted(&items, &ItemsModel::rowsInserted);
            QVERIFY(rowsInserted.wait());
            QCOMPARE(items.rowCount(), 1);
        }
    }

    // Sync without changes should be "invariant"
    davClient->synchronize();
    davClient->synchronize();
    davClient->synchronize();

    {
        Library library2(dir2.path());
        QTemporaryDir tmpDir;
        Cache cache;
        cache.setCacheDirectory(tmpDir.path());
        QVERIFY(cache.open());
        auto q = new LoadLibraryQuery;
        q->setLibraryId(library.uid());
        q->setDirectory(dir2.path());
        QSignalSpy spy(q, &LoadLibraryQuery::finished);
        cache.run(q);
        QVERIFY(spy.wait());

        {
            LibrariesModel libraries;
            libraries.setCache(&cache);
            QSignalSpy rowsInserted(&libraries, &LibrariesModel::rowsInserted);
            QVERIFY(rowsInserted.wait());
            QCOMPARE(libraries.rowCount(), 1);
        }

        {
            ItemsModel items;
            items.setCache(&cache);
            items.setParentItem(library.uid());
            QSignalSpy rowsInserted(&items, &ItemsModel::rowsInserted);
            QVERIFY(rowsInserted.wait());
            QCOMPARE(items.rowCount(), 3);
        }

        {
            ItemsModel items;
            items.setCache(&cache);
            items.setParentItem(todoList.uid());
            QSignalSpy rowsInserted(&items, &ItemsModel::rowsInserted);
            QVERIFY(rowsInserted.wait());
            QCOMPARE(items.rowCount(), 1);
        }

        {
            ItemsModel items;
            items.setCache(&cache);
            items.setParentItem(todo.uid());
            QSignalSpy rowsInserted(&items, &ItemsModel::rowsInserted);
            QVERIFY(rowsInserted.wait());
            QCOMPARE(items.rowCount(), 1);
        }
    }
}

void WebDAVSynchronizerTest::synchronize_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::createDavClients()
{
    QTest::addColumn<QObject*>("client");

    const auto servers = readConfiguredWebDAVTestServers();
    int count = 0;
    for (const auto& server : servers) {
        auto client = new WebDAVSynchronizer;
        client->setUrl(server.url.toString());
        client->setUsername(server.url.userName());
        client->setPassword(server.url.password());
        switch (server.type) {
        case WebDAVAccount::WebDAV:
            client->setServerType(WebDAVSynchronizer::Generic);
            break;
        case WebDAVAccount::NextCloud:
            client->setServerType(WebDAVSynchronizer::NextCloud);
            break;
        case WebDAVAccount::OwnCloud:
            client->setServerType(WebDAVSynchronizer::OwnCloud);
            break;
        default:
            qWarning() << "Invalid WebDAV account type:" << server.type;
            break;
        }

        QTest::newRow(server.name.toUtf8()) << static_cast<QObject*>(client);
        ++count;
    }
    if (count == 0) {
        QSKIP("No suitable WebDAV servers configured");
    }
}

void WebDAVSynchronizerTest::echoToFile(const QString& text, const QString& filename)
{
    QFile file(filename);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(text.toUtf8());
    file.close();
}

QByteArray WebDAVSynchronizerTest::catFile(const QString& filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    auto result = file.readAll();
    file.close();
    return result;
}

QTEST_MAIN(WebDAVSynchronizerTest)
#include "test_webdavsynchronizer.moc"
