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
#include <QSettings>
#include <QTemporaryDir>
#include <QTest>

#include "utils/keystore.h"

class KeyStoreTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void testWriteRead();
    void cleanup() {}
    void cleanupTestCase() {}
};

void KeyStoreTest::testWriteRead()
{
    QTemporaryDir tempDir;
    auto settings = KeyStore::createSettingsFile(tempDir.path() + "/test.otlks");
    QVERIFY(settings != nullptr);
    settings->beginGroup("Test");
    int foo = 10;
    QString bar = "Hello World";
    settings->setValue("foo", foo);
    settings->setValue("bar", bar);
    delete settings;

    settings = KeyStore::createSettingsFile(tempDir.path() + "/test.otlks");
    QVERIFY(settings != nullptr);
    settings->beginGroup("Test");
    QCOMPARE(settings->value("foo").toInt(), foo);
    QCOMPARE(settings->value("bar").toString(), bar);
    delete settings;
}

QTEST_MAIN(KeyStoreTest)
#include "test_keystore.moc"
