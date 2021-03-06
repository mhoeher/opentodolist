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

#define WEBDAV_SYNCHRONIZER_TEST
#include "sync/webdavsynchronizer.h"

#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/image.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/task.h"
#include "datastorage/cache.h"
#include "datastorage/libraryloader.h"
#include "models/itemsmodel.h"
#include "models/librariesmodel.h"

#if defined(OPENTODOLIST_NEXTCLOUD_TEST_URL) || defined(OPENTODOLIST_OWNCLOUD_TEST_URL)
#    define TEST_AGAINST_SERVER
#endif

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

#ifdef TEST_AGAINST_SERVER
    void validate();
    void validate_data();
    void synchronize();
    void synchronize_data();
    void findExistingEntries();
    void findExistingEntries_data();
#endif // TEST_AGAINST_SERVER
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

#ifdef TEST_AGAINST_SERVER

void WebDAVSynchronizerTest::validate()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    Q_CHECK_PTR(davClient);
    QCOMPARE(davClient->valid(), false);
    QCOMPARE(davClient->validating(), false);
    QSignalSpy validatingChanged(davClient, &Synchronizer::validatingChanged);
    QSignalSpy validChanged(davClient, &Synchronizer::validChanged);
    davClient->validate();
    QVERIFY(validatingChanged.wait(30000));
    QCOMPARE(validatingChanged.count(), 2);
    QCOMPARE(validChanged.count(), 1);
    QCOMPARE(davClient->valid(), true);
    QCOMPARE(davClient->validating(), false);
}

void WebDAVSynchronizerTest::validate_data()
{
    createDavClients();
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
    connect(davClient, &WebDAVSynchronizer::synchronizingChanged,
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
        LibraryLoader loader;
        loader.setLibraryId(library.uid());
        loader.setCache(&cache);
        loader.setDirectory(dir2.path());
        loader.scan();
        QSignalSpy spy(&loader, &LibraryLoader::scanFinished);
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
        LibraryLoader loader;
        loader.setLibraryId(library.uid());
        loader.setCache(&cache);
        loader.setDirectory(dir2.path());
        loader.scan();
        QSignalSpy spy(&loader, &LibraryLoader::scanFinished);
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

void WebDAVSynchronizerTest::findExistingEntries()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);

    QTemporaryDir dir1;
    QTemporaryDir dir2;
    QList<QUuid> uids;

    {
        Library lib(dir1.path());
        lib.setName("foo");
        lib.save();
        uids.append(lib.uid());
    }
    {
        Library lib(dir2.path());
        lib.setName("bar");
        lib.save();
        uids.append(lib.uid());
    }

    auto dirName = QUuid::createUuid().toString() + "-" + __func__;

    davClient->setDirectory(dir1.path());
    davClient->setRemoteDirectory(dirName + "/lib1.otl");
    davClient->synchronize();

    davClient->setDirectory(dir2.path());
    davClient->setRemoteDirectory(dirName + "/OpenTodoList/lib2.otl");
    davClient->synchronize();

    davClient->setRemoteDirectory(dirName);
    QSignalSpy spy(davClient, &WebDAVSynchronizer::existingLibrariesChanged);
    davClient->findExistingLibraries();
    spy.wait();
    auto existingLibs = davClient->existingLibraries();
    QCOMPARE(existingLibs.length(), 2);
    SynchronizerExistingLibrary lib1 = existingLibs.at(0).value<SynchronizerExistingLibrary>();
    SynchronizerExistingLibrary lib2 = existingLibs.at(1).value<SynchronizerExistingLibrary>();
    QVERIFY(lib1.name() == "foo" || lib2.name() == "foo");
    QVERIFY(lib1.name() == "bar" || lib2.name() == "bar");
    QVERIFY(lib1.name() != lib2.name());
    QVERIFY(uids.contains(lib1.uid()));
    QVERIFY(uids.contains(lib2.uid()));
    QVERIFY(lib1.uid() != lib2.uid());
    for (auto lib : existingLibs) {
        auto l = lib.value<SynchronizerExistingLibrary>();
        if (l.name() == "foo") {
            QVERIFY(l.path() == "/lib1.otl");
        } else {
            QVERIFY(l.path() == "/OpenTodoList/lib2.otl");
        }
    }
}

void WebDAVSynchronizerTest::findExistingEntries_data()
{
    createDavClients();
}

#endif // TEST_AGAINST_SERVER

void WebDAVSynchronizerTest::createDavClients()
{
    QTest::addColumn<QObject*>("client");

#ifdef OPENTODOLIST_NEXTCLOUD_TEST_URL
    {
        auto url = QUrl(OPENTODOLIST_NEXTCLOUD_TEST_URL);
        auto client = new WebDAVSynchronizer();
        client->setServerType(WebDAVSynchronizer::NextCloud);
        client->setUrl(url.toString(QUrl::RemoveUserInfo | QUrl::PrettyDecoded));
        client->setUsername(url.userName());
        client->setPassword(url.password());
        if (client->url().scheme() == "http") {
            client->setDisableCertificateCheck(true);
        }
        QTest::newRow("NextCloud") << static_cast<QObject*>(client);
    }
#endif

#ifdef OPENTODOLIST_OWNCLOUD_TEST_URL
    {
        auto url = QUrl(OPENTODOLIST_OWNCLOUD_TEST_URL);
        auto client = new WebDAVSynchronizer();
        client->setServerType(WebDAVSynchronizer::OwnCloud);
        client->setUrl(url.toString(QUrl::RemoveUserInfo | QUrl::PrettyDecoded));
        client->setUsername(url.userName());
        client->setPassword(url.password());
        if (client->url().scheme() == "http") {
            client->setDisableCertificateCheck(true);
        }
        QTest::newRow("ownCloud") << static_cast<QObject*>(client);
    }
#endif
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
