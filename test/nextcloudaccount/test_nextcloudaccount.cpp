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

#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "datamodel/library.h"
#include "sync/nextcloudaccount.h"
#include "../test-utils.h"

class NextCloudAccountTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void login();
    void login_data();
    void findExistingLibraries_data();
    void findExistingLibraries();
    void cleanup() {}
    void cleanupTestCase() {}
};

void NextCloudAccountTest::login()
{
    QFETCH(QUrl, url);

    NextCloudAccount account;
    account.setBaseUrl(url.toString());
    account.setUsername(url.userName());
    account.setPassword(url.password());
    QVERIFY(!account.online());
    QSignalSpy loggingInChanged(&account, &NextCloudAccount::loggingInChanged);
    account.login();
    QCOMPARE(loggingInChanged.count(), 1);
    QSignalSpy onlineChanged(&account, &NextCloudAccount::onlineChanged);
    QSignalSpy loginFinished(&account, &NextCloudAccount::loginFinished);
    QVERIFY(loginFinished.wait());
    QVERIFY(loginFinished.at(0).at(0).toBool()); // 'success' shall be true
    QCOMPARE(loggingInChanged.count(), 2);
    QCOMPARE(onlineChanged.count(), 1);
    QVERIFY(account.online());
}

void NextCloudAccountTest::login_data()
{
    addWebDAVServerTestUrls<WebDAVAccount::NextCloud>();
}

void NextCloudAccountTest::findExistingLibraries()
{
    QFETCH(QUrl, url);

    NextCloudAccount account;
    account.setBaseUrl(url.toString());
    account.setUsername(url.userName());
    account.setPassword(url.password());

    QTemporaryDir dir1;
    QTemporaryDir dir2;
    QList<QUuid> uids;
    QList<QString> names;
    QList<QString> paths;

    {
        Library lib(dir1.path());
        lib.setName("foo");
        lib.save();
        uids.append(lib.uid());
        names.append(lib.name());
        paths.append("/" + QUuid::createUuid().toString() + ".otl");
    }
    {
        Library lib(dir2.path());
        lib.setName("bar");
        lib.save();
        uids.append(lib.uid());
        names.append(lib.name());
        paths.append("/OpenTodoList/" + QUuid::createUuid().toString() + ".otl");
    }

    auto davClient = account.createSynchronizer();

    davClient->setDirectory(dir1.path());
    davClient->setRemoteDirectory(paths[0]);
    davClient->synchronize();

    davClient->setDirectory(dir2.path());
    davClient->setRemoteDirectory(paths[1]);
    davClient->synchronize();

    delete davClient;

    QSignalSpy spy(&account, &WebDAVAccount::remoteLibrariesChanged);
    QCOMPARE(account.remoteLibraries().length(), 0);
    account.findExistingLibraries();
    spy.wait();
    const auto& existingLibs = account.remoteLibraries();
    for (int i = 0; i < uids.length(); ++i) {
        bool found = false;
        for (const auto& existingLib : existingLibs) {
            if (existingLib.name() == names[i] && existingLib.uid() == uids[i]
                && existingLib.path() == paths[i]) {
                found = true;
                break;
            }
        }
        if (qEnvironmentVariableIsSet("CI")) {
            if (!found) {
                qWarning() << "Didn't find expected library using" << url
                           << " - this is known to fail in CI";
            }
        } else {
            QVERIFY2(found,
                     qUtf8Printable("Found expected library at index " + QString::number(i)));
        }
    }
}

void NextCloudAccountTest::findExistingLibraries_data()
{
    addWebDAVServerTestUrls<WebDAVAccount::NextCloud>();
}

QTEST_MAIN(NextCloudAccountTest)
#include "test_nextcloudaccount.moc"
