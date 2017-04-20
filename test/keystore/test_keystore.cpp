#include "utils/keystore.h"

#include <QObject>
#include <QSettings>
#include <QTemporaryDir>
#include <QTest>

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
