#include "application.h"
#include "image.h"
#include "library.h"
#include "locallibraryfactory.h"
#include "note.h"
#include "todolist.h"

#include <QObject>
#include <QSettings>
#include <QTemporaryDir>
#include <QTest>

class LibraryTest : public QObject 
{
  
  Q_OBJECT
  
private:
  
  QTemporaryDir   *m_appDir;
  QSettings       *m_appSettings;
  Application     *m_app;
  QTemporaryDir   *m_dir;
  Library         *m_library;
  
private slots:
  
  void init();
  void testAddNote();
  void testAddImage();
  void testAddTodoList();
  void testNotePersistence();
  void testImagePersistence();
  void testTodoListPersistence();
  void testDeleteNote();
  void testDeleteImage();
  void testDeleteTodoList();
  void cleanup();
  
};


void LibraryTest::init()
{
  m_appDir = new QTemporaryDir();
  Q_CHECK_PTR(m_appDir);
  QVERIFY2(m_appDir->isValid(), "Failed to create temporary directory.");
  m_appSettings = new QSettings(m_appDir->path() + "/settings.ini", QSettings::IniFormat);
  Q_CHECK_PTR(m_appSettings);
  m_app = new Application(m_appSettings);
  Q_CHECK_PTR(m_app);
  m_dir = new QTemporaryDir();
  QVERIFY2(m_dir->isValid(), "Failed to create temporary directory.");
  m_library = m_app->addLibrary(LocalLibraryFactory::ID, "Test Library", m_dir->path());
  Q_CHECK_PTR(m_library);
}

void LibraryTest::testAddNote()
{
  auto note = m_library->addNote("Test Note");
  Q_CHECK_PTR(note);
  QCOMPARE(note->title(), QString("Test Note"));
  QCOMPARE(m_library->items().size(), 1);
  Q_CHECK_PTR(m_library->items().at(0));
  QCOMPARE(m_library->items().at(0), note);
}

void LibraryTest::testAddImage()
{
  auto image = m_library->addImage("Test Image", "image.png");
  Q_CHECK_PTR(image);
  QCOMPARE(image->title(), QString("Test Image"));
  QCOMPARE(image->image(), QString("image.png"));
  QCOMPARE(m_library->items().size(), 1);
  Q_CHECK_PTR(m_library->items().at(0));
  QCOMPARE(m_library->items().at(0), image);
}

void LibraryTest::testAddTodoList()
{
  auto todoList = m_library->addTodoList("Test Todo List");
  Q_CHECK_PTR(todoList);
  QCOMPARE(todoList->title(), QString("Test Todo List"));
  QCOMPARE(m_library->items().size(), 1);
  Q_CHECK_PTR(m_library->items().at(0));
  QCOMPARE(m_library->items().at(0), todoList);
}

void LibraryTest::testNotePersistence()
{
  m_library->addNote("Test Note");
  auto factory = m_app->libraryFactoryForType(LocalLibraryFactory::ID);
  Q_CHECK_PTR(factory);
  auto library = factory->createLibrary("Test Library", m_dir->path());
  QCOMPARE(library->items().size(), 1);
  auto item = library->items().at(0);
  Q_CHECK_PTR(item);
  auto note = dynamic_cast<Note*>(item);
  Q_CHECK_PTR(note);
  QCOMPARE(note->title(), QString("Test Note"));
}

void LibraryTest::testImagePersistence()
{
  m_library->addImage("Test Image", "image.png");
  auto factory = m_app->libraryFactoryForType(LocalLibraryFactory::ID);
  Q_CHECK_PTR(factory);
  auto library = factory->createLibrary("Test Library", m_dir->path());
  QCOMPARE(library->items().size(), 1);
  auto item = library->items().at(0);
  Q_CHECK_PTR(item);
  auto image = dynamic_cast<Image*>(item);
  Q_CHECK_PTR(image);
  QCOMPARE(image->title(), QString("Test Image"));
  QCOMPARE(image->image(), QString("image.png"));
}

void LibraryTest::testTodoListPersistence()
{
  m_library->addTodoList("Test Todo List");
  auto factory = m_app->libraryFactoryForType(LocalLibraryFactory::ID);
  Q_CHECK_PTR(factory);
  auto library = factory->createLibrary("Test Library", m_dir->path());
  QCOMPARE(library->items().size(), 1);
  auto item = library->items().at(0);
  Q_CHECK_PTR(item);
  auto todoList = dynamic_cast<TodoList*>(item);
  Q_CHECK_PTR(todoList);
  QCOMPARE(todoList->title(), QString("Test Todo List"));
}

void LibraryTest::testDeleteNote()
{
  auto note = m_library->addNote("Test Note");
  Q_CHECK_PTR(note);
  QCOMPARE(m_library->items().size(), 1);
  note->deleteItem();
  QCOMPARE(m_library->items().size(), 0);
}

void LibraryTest::testDeleteImage()
{
  auto image = m_library->addImage("Test Image", "image.png");
  Q_CHECK_PTR(image);
  QCOMPARE(m_library->items().size(), 1);
  image->deleteItem();
  QCOMPARE(m_library->items().size(), 0);
}

void LibraryTest::testDeleteTodoList()
{
  auto todoList = m_library->addTodoList("Test Todo List");
  Q_CHECK_PTR(todoList);
  QCOMPARE(m_library->items().size(), 1);
  todoList->deleteItem();
  QCOMPARE(m_library->items().size(), 0);
}

void LibraryTest::cleanup()
{
  delete m_library;
  delete m_dir;
}

QTEST_MAIN(LibraryTest)
#include "test_library.moc"
