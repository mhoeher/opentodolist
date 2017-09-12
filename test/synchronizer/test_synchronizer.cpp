#include "sync/synchronizer.h"
#include "sync/webdavsynchronizer.h"
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
    for (auto serverType : {WebDAVSynchronizer::NextCloud,
            WebDAVSynchronizer::OwnCloud, WebDAVSynchronizer::Generic}) {
        QTemporaryDir dir;
        {
            WebDAVSynchronizer sync;
            sync.setServerType(serverType);
            sync.setDirectory(dir.path());
            QVERIFY(sync.save());
        }
        {
            auto sync = Synchronizer::fromDirectory(dir.path());
            Q_CHECK_PTR(sync);
            auto s = dynamic_cast<WebDAVSynchronizer*>(sync);
            QVERIFY(s != nullptr);
            QCOMPARE(s->serverType(), serverType);
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
