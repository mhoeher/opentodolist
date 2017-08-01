#include "sync/genericdavsynchronizer.h"

#include <QObject>
#include <QTest>

class GenericDAVSynchronizerTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void cleanup() {}
  void cleanupTestCase() {}
};


QTEST_MAIN(GenericDAVSynchronizerTest)
#include "test_genericdavsynchronizer.moc"
