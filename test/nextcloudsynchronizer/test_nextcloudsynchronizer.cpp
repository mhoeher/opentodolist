#include "sync/nextcloudsynchronizer.h"

#include <QObject>
#include <QTest>

class NextCloudSynchronizerTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void cleanup() {}
  void cleanupTestCase() {}
};


QTEST_MAIN(NextCloudSynchronizerTest)
#include "test_nextcloudsynchronizer.moc"
