#include "application.h"

#include <QObject>
#include <QTest>

class ApplicationTest : public QObject
{
  Q_OBJECT
private slots:
  
  void initTestCase();
  void testDefaultConstructor();
  void cleanupTestCase();
  
};

void ApplicationTest::initTestCase()
{
  // Nothing to be done here
}

void ApplicationTest::testDefaultConstructor()
{
  Application *app = new Application();
  Q_CHECK_PTR(app);
  delete app;
}

void ApplicationTest::cleanupTestCase()
{
  // Nothing to be done here
}

QTEST_MAIN(ApplicationTest)
#include "test_application.moc"
