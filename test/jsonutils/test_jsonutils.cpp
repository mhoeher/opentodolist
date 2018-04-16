#include "utils/jsonutils.h"

#include <QFile>
#include <QObject>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QTest>


using JsonUtils::patchJsonFile;
using JsonUtils::loadMap;


class JsonUtilsTest : public QObject
{
  Q_OBJECT


private slots:

  void initTestCase() {}
  void init() {}
  void testLoadMap();
  void testLoadMapWithNonExistingFile();
  void testLoadMapWithInvalidFile();
  void testPatchJsonFile();
  void cleanup() {}
  void cleanupTestCase() {}
};




void JsonUtilsTest::testLoadMap()
{
    QTemporaryDir tempDir;
    auto filename = tempDir.path() + "/test.json";
    QFile file(filename);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(QString("{ \"Foo\": \"Hello World\", \"Bar\": 10 }").toUtf8());
    file.close();
    bool ok;
    auto map = loadMap(filename, &ok);
    QVERIFY(ok);
    QCOMPARE(map.value("Foo").toString(), QString("Hello World"));
    QCOMPARE(map.value("Bar").toInt(), 10);
}

void JsonUtilsTest::testLoadMapWithNonExistingFile()
{
    QTemporaryDir tempDir;
    auto filename = tempDir.path() + "/test.json";
    bool ok;
    auto map = loadMap(filename, &ok);
    QVERIFY(!ok);
    QCOMPARE(map.size(), 0);
}

void JsonUtilsTest::testLoadMapWithInvalidFile()
{
    QTemporaryDir tempDir;
    auto filename = tempDir.path() + "/test.json";
    QFile file(filename);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(QString("{\"Foo\": Hello World, \"Bar\": 10").toUtf8());
    file.close();
    bool ok;
    QTest::ignoreMessage(QtWarningMsg,
                         QRegularExpression(
                             ".*Failed to parse.*illegal number.*"));
    auto map = loadMap(filename, &ok);
    QVERIFY(!ok);
    QCOMPARE(map.size(), 0);
}

void JsonUtilsTest::testPatchJsonFile()
{
    QTemporaryDir tempDir;
    auto filename = tempDir.path() + "/test.json";
    QVariantMap v1;
    v1["Foo"] = "Hello";
    QVERIFY(patchJsonFile(filename, v1));
    QVariantMap v2;
    v2["Bar"] = "World";
    QVERIFY(patchJsonFile(filename, v2));
    auto merged = loadMap(filename);
    QCOMPARE(merged.value("Foo").toString(), QString("Hello"));
    QCOMPARE(merged.value("Bar").toString(), QString("World"));
    v2["Bar"] = "Baz";
    QVERIFY(patchJsonFile(filename, v2));
    merged = loadMap(filename);
    QCOMPARE(merged.value("Bar").toString(), QString("Baz"));
}

QTEST_MAIN(JsonUtilsTest)
#include "test_jsonutils.moc"
