#include "itemsmodel.h"

#include <QObject>
#include <QTest>

class ItemsModelTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void cleanup() {}
  void cleanupTestCase() {}
};


QTEST_MAIN(ItemsModelTest)
#include "test_itemsmodel.moc"
