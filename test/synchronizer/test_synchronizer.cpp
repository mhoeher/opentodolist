#include "sync/synchronizer.h"
#include "sync/nextcloudsynchronizer.h"
#include "sync/genericdavsynchronizer.h"
#include "utils/jsonutils.h"

#include <QObject>
#include <QTest>
#include <QTemporaryDir>

class SynchronizerTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void fromDirectory();
  void cleanup() {}
  void cleanupTestCase() {}
};




void SynchronizerTest::fromDirectory()
{
    {
        QTemporaryDir dir;
        {
            NextCloudSynchronizer sync;
            sync.setDirectory(dir.path());
            QVERIFY(sync.save());
        }
        {
            auto sync = Synchronizer::fromDirectory(dir.path());
            Q_CHECK_PTR(sync);
            QVERIFY(dynamic_cast<NextCloudSynchronizer*>(sync) != nullptr);
            delete sync;
        }
    }
    {
        QTemporaryDir dir;
        {
            GenericDAVSynchronizer sync;
            sync.setDirectory(dir.path());
            QVERIFY(sync.save());
        }
        {
            auto sync = Synchronizer::fromDirectory(dir.path());
            Q_CHECK_PTR(sync);
            QVERIFY(dynamic_cast<GenericDAVSynchronizer*>(sync) != nullptr);
            delete sync;
        }
    }
    {
        QTemporaryDir dir;
        QVariantMap map;
        map["type"] = "foo";
        JsonUtils::patchJsonFile(dir.path() + "/" + Synchronizer::SaveFileName,
                                 map);
        auto sync = Synchronizer::fromDirectory(dir.path());
        QVERIFY(sync == nullptr);
    }
}

QTEST_MAIN(SynchronizerTest)
#include "test_synchronizer.moc"
