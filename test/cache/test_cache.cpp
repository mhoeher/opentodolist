#include "datastorage/cache.h"

#include <QObject>
#include <QTest>
#include <QTemporaryDir>

class CacheTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init();
  void constructor();
  void cacheDirectory();
  void open();
  void cleanup();
  void cleanupTestCase() {}

private:

  QTemporaryDir *tempDir;

  static const size_t CACHE_SIZE = 1024 * 1024;
};


void CacheTest::init() {
    tempDir = new QTemporaryDir();
}

void CacheTest::constructor()
{
    auto c = new Cache();
    delete c;
}

void CacheTest::cacheDirectory()
{
    Cache c;
    c.setCacheDirectory(tempDir->path());
    QCOMPARE(c.cacheDirectory(), tempDir->path());
}

void CacheTest::open()
{
    {
        Cache c;
        QVERIFY(!c.isValid());
        QTest::ignoreMessage(QtWarningMsg, "Failed to open cache directory \"\""
                                           " : \"Empty path passed to "
                                           "environment\"");
        QVERIFY(!c.open());
        QVERIFY(!c.isValid());
    }
    {
        Cache c;
        QVERIFY(!c.isValid());
        c.setCacheDirectory(tempDir->path());
        c.setCacheSize(CACHE_SIZE);
        QTest::ignoreMessage(QtDebugMsg,
                             "Cache is uninitialized - "
                             "initializing to Version 0 now");
        QVERIFY(c.open());
        QVERIFY(c.isValid());
    }
}

void CacheTest::cleanup() {
    delete tempDir;
}

QTEST_MAIN(CacheTest)
#include "test_cache.moc"
