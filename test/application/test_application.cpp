#include "application.h"

#include "locallibraryfactory.h"

#include <QObject>
#include <QTest>

class ApplicationTest : public QObject
{
  Q_OBJECT

private slots:  
  void initTestCase();
  void testDefaultConstructor();
  void testLibraryFactories();
  void testLibraryTypes();
  void testLibraryFactoryForType();
  void cleanupTestCase();
  
};

void ApplicationTest::initTestCase()
{
  // Nothing to be done here
}

/**
   @brief Ensure that the application is default-constructable.
 */
void ApplicationTest::testDefaultConstructor()
{
  Application *app = new Application();
  Q_CHECK_PTR(app);
  delete app;
}

/**
   @brief Check if the list of LibraryFactories contains the expected factories.
 */
void ApplicationTest::testLibraryFactories()
{
  Application app;
  auto factories = app.libraryFactories();
  bool localLibraryFactoryPresent = false;
  for (auto factory : factories) {
    if (dynamic_cast<LocalLibraryFactory*>(factory) != nullptr) {
      localLibraryFactoryPresent = true;
    }
  }
  QVERIFY2(localLibraryFactoryPresent, "LocalLibraryFactory is not present in the Application.");
}

/**
   @brief Check if the libraryTypes() method returns the expected factory IDs.
 */
void ApplicationTest::testLibraryTypes()
{
  Application app;
  auto ids = app.libraryTypes();
  QCOMPARE(ids, QStringList({LocalLibraryFactory::ID}));
}

/**
   @brief Check if the libraryFactoryForType() works as expected.
 */
void ApplicationTest::testLibraryFactoryForType()
{
  Application app;
  QVERIFY2(dynamic_cast<LocalLibraryFactory*>(app.libraryFactoryForType(LocalLibraryFactory::ID)),
           "libraryFactoryForType(LocalLibraryFactory) does not return a LocalLibraryFactory.");
  QVERIFY2(app.libraryFactoryForType("DefinitelyNonExistingId") == nullptr,
           "libraryFactoryForType(NonExistingType) does not return a nullptr.");
}

void ApplicationTest::cleanupTestCase()
{
  // Nothing to be done here
}

QTEST_MAIN(ApplicationTest)
#include "test_application.moc"
