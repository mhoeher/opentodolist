#include "application.h"

#include "locallibraryfactory.h"

#include <QObject>
#include <QTemporaryDir>
#include <QTest>

class ApplicationTest : public QObject
{
  Q_OBJECT

  QTemporaryDir *m_appDir;
  QSettings     *m_settings;
  Application   *m_app;
  
private slots:  
  void init();
  void testLibraryFactories();
  void testLibraryTypes();
  void testLibraryFactoryForType();
  void testAddLibrary();
  void testAddLocalLibrary();
  void testAddLocalLibraryWithoutName();
  void testLibraries();
  void testAddLibraryWithWrongFactoryType();
  void testLibraryPersistence();
  void testDeleteLibrary();
  void cleanup();
  
};

void ApplicationTest::init()
{
  m_appDir = new QTemporaryDir();
  Q_CHECK_PTR(m_appDir);
  QVERIFY2(m_appDir->isValid(), "Failed to create temporary directory.");
  m_settings = new QSettings(m_appDir->path() + "/settings.ini", QSettings::IniFormat);
  Q_CHECK_PTR(m_settings);
  m_app = new Application(m_settings);
  Q_CHECK_PTR(m_app);
}

/**
   @brief Check if the list of LibraryFactories contains the expected factories.
 */
void ApplicationTest::testLibraryFactories()
{
  auto factories = m_app->libraryFactories();
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
  auto ids = m_app->libraryTypes();
  QCOMPARE(ids, QStringList({LocalLibraryFactory::ID}));
}

/**
   @brief Check if the libraryFactoryForType() works as expected.
 */
void ApplicationTest::testLibraryFactoryForType()
{
  QVERIFY2(dynamic_cast<LocalLibraryFactory*>(m_app->libraryFactoryForType(LocalLibraryFactory::ID)),
           "libraryFactoryForType(LocalLibraryFactory) does not return a LocalLibraryFactory.");
  QVERIFY2(m_app->libraryFactoryForType("DefinitelyNonExistingId") == nullptr,
           "libraryFactoryForType(NonExistingType) does not return a nullptr.");
}

/**
   @brief The Application shall be able to create a new library.
 */
void ApplicationTest::testAddLibrary()
{
  auto library = m_app->addLibrary(LocalLibraryFactory::ID, "New Library", m_appDir->path());
  Q_CHECK_PTR(library);
}

/**
   @brief The Application shall be able to open a local directory as library.
 */
void ApplicationTest::testAddLocalLibrary()
{
  auto library = m_app->addLocalLibrary("New Library", m_appDir->path());
  Q_CHECK_PTR(library);
}

/**
   @brief The Application shall be able to open a local directory and create a name for it.
 */
void ApplicationTest::testAddLocalLibraryWithoutName()
{
  auto library = m_app->addLocalLibrary(m_appDir->path() + "/Test Library");
  Q_CHECK_PTR(library);
  QCOMPARE(library->name(), QString("Test Library"));
}

/**
   @brief Make sure that the libraries() method returns the libraries we added.
 */
void ApplicationTest::testLibraries()
{
  auto library = m_app->addLibrary(LocalLibraryFactory::ID, "New Library", m_appDir->path());
  Q_CHECK_PTR(library);
  QCOMPARE(m_app->libraries().size(), 1);
  QCOMPARE(m_app->libraries().at(0), library);
}

/**
   @brief If called with a non-existent factory type, addLibrary() shall return a nullptr.
 */
void ApplicationTest::testAddLibraryWithWrongFactoryType()
{
  auto library = m_app->addLibrary("", m_appDir->path(), "New Library");
  QVERIFY2(!library, "Expected library to be nullptr.");
}

void ApplicationTest::testLibraryPersistence()
{
  auto library = m_app->addLibrary(LocalLibraryFactory::ID, "New Library", m_appDir->path());
  Q_CHECK_PTR(library);
  QCOMPARE(m_app->libraries().size(), 1);
  delete m_app;
  m_app = new Application(m_settings);
  Q_CHECK_PTR(m_app);
  QCOMPARE(m_app->libraries().size(), 1);
  library = m_app->libraries().at(0);
  Q_CHECK_PTR(library);
  QCOMPARE(library->name(), QString("New Library"));
}

void ApplicationTest::testDeleteLibrary()
{
  auto library = m_app->addLibrary(LocalLibraryFactory::ID, "New Library", m_appDir->path());
  Q_CHECK_PTR(library);
  QCOMPARE(m_app->libraries().size(), 1);
  delete m_app;
  m_app = new Application(m_settings);
  Q_CHECK_PTR(m_app);
  QCOMPARE(m_app->libraries().size(), 1);
  library = m_app->libraries().at(0);
  Q_CHECK_PTR(library);
  library->deleteLibrary();
  QCOMPARE(m_app->libraries().length(), 0);
  delete m_app;
  m_app = new Application(m_settings);
  Q_CHECK_PTR(m_app);
  QCOMPARE(m_app->libraries().size(), 0);
}

void ApplicationTest::cleanup()
{
  delete m_app;
  delete m_settings;
  delete m_appDir;
}

QTEST_MAIN(ApplicationTest)
#include "test_application.moc"
