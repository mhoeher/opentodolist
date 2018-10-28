#include <QObject>
#include <QRegularExpression>
#include <QSet>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "datamodel/library.h"
#include "datastorage/cache.h"
#include "datastorage/getlibraryquery.h"
#include "datastorage/insertorupdateitemsquery.h"

class GetLibraryQueryTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void run();
  void cleanup() {}
  void cleanupTestCase() {}
};




void GetLibraryQueryTest::run()
{
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024*1024);
    QVERIFY(cache.open());

    Library lib;
    lib.setName("A library");

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new GetLibraryQuery();
        q->setUid(lib.uid());
        QSignalSpy libraryLoaded(q, &GetLibraryQuery::libraryLoaded);
        cache.run(q);
        QVERIFY(libraryLoaded.wait());
        QSharedPointer<Library> l(
                    Library::decache(libraryLoaded.value(0).value(0)));
        QVERIFY(l != nullptr);
        QCOMPARE(l->uid(), lib.uid());
        QCOMPARE(l->name(), lib.name());
    }
}

QTEST_MAIN(GetLibraryQueryTest)
#include "test_getlibraryquery.moc"
