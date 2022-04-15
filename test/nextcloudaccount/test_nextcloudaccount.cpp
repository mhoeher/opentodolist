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

QTEST_MAIN(NextCloudAccountTest)
#include "test_nextcloudaccount.moc"
