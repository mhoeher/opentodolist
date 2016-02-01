#include "application.h"
#include "locallibraryfactory.h"

#include <QTemporaryDir>
#include <QTest>

class LocalLibraryFactoryTest : public QObject
{
  Q_OBJECT
 
private:
  Application *app;
  
private slots:
  void initTestCase();
  void testProperties();
  void testCreateLibrary();
  void cleanupTestCase();
};



void LocalLibraryFactoryTest::initTestCase() {
  app = new Application();
  Q_CHECK_PTR(app);
}

void LocalLibraryFactoryTest::testProperties()
{
  auto factory = app->libraryFactoryForType(LocalLibraryFactory::ID);
  Q_CHECK_PTR(factory);
  QVERIFY2(!factory->name().isEmpty(), "LocalLibraryFactory name must not be empty.");
  QVERIFY2(!factory->description().isEmpty(), "LocalLibraryFactory description most not be empty.");
  QVERIFY2(!factory->id().isEmpty(), "LocalLibraryFactory id must not be empty.");
  QVERIFY2(!factory->createComponent().isEmpty(), 
           "LocalLibraryFactory createComponent must not be empty.");
  QVERIFY2(!factory->editComponent().isEmpty(),
           "LocalLibraryFactory editComponen must not be empty.");
}

void LocalLibraryFactoryTest::testCreateLibrary()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Unable to create temporary directory.");
  auto library = app->libraryFactoryForType(LocalLibraryFactory::ID)->createLibrary(
        "Test", dir.path());
  Q_CHECK_PTR(library);
  QCOMPARE(library->name(),QString("Test"));
  QCOMPARE(library->directory(), dir.path());
}

void LocalLibraryFactoryTest::cleanupTestCase() {
  delete app;
}

QTEST_MAIN(LocalLibraryFactoryTest)
#include "test_locallibraryfactory.moc"
