#include "application.h"
#include "image.h"
#include "library.h"
#include "note.h"
#include "todolist.h"

#include <QObject>
#include <QSettings>
#include <QTemporaryDir>
#include <QTest>

class LibraryTest : public QObject
{

    Q_OBJECT

private slots:

    void init();
    void testAddNote();
    void testAddImage();
    void testAddImageWithoutTitle();
    void testAddTodoList();
    void testNotePersistence();
    void testImagePersistence();
    void testTodoListPersistence();
    void testDeleteNote();
    void testDeleteImage();
    void testDeleteTodoList();
    void testTags();
    void cleanup();

};


void LibraryTest::init()
{
    // TODO: Implement me
}

void LibraryTest::testAddNote()
{
    // TODO: Implement me
}

void LibraryTest::testAddImage()
{
    // TODO: Implement me
}

void LibraryTest::testAddImageWithoutTitle()
{
    // TODO: Implement me
}

void LibraryTest::testAddTodoList()
{
    // TODO: Implement me
}

void LibraryTest::testNotePersistence()
{
    // TODO: Implement me
}

void LibraryTest::testImagePersistence()
{
    // TODO: Implement me
}

void LibraryTest::testTodoListPersistence()
{
    // TODO: Implement me
}

void LibraryTest::testDeleteNote()
{
    // TODO: Implement me
}

void LibraryTest::testDeleteImage()
{
    // TODO: Implement me
}

void LibraryTest::testDeleteTodoList()
{
    // TODO: Implement me
}

void LibraryTest::testTags()
{
    // TODO: Implement me
}

void LibraryTest::cleanup()
{
    // TODO: Implement me
}

QTEST_MAIN(LibraryTest)
#include "test_library.moc"
