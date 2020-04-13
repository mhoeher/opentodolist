#include "application.h"
#include "datamodel/image.h"
#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datastorage/cache.h"
#include "models/itemsmodel.h"

#include <QObject>
#include <QQmlEngine>
#include <QSet>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <iostream>

class LibraryTest : public QObject
{

    Q_OBJECT

private slots:

    void init();
    void testProperties();
    void testVariant();
    void testLoad();
    void testFromJson();
    void testEncache();
    void testDecache();
    void cleanup();

private:
    QTemporaryDir *m_dir;
};

void LibraryTest::init()
{
    m_dir = new QTemporaryDir();
}

void LibraryTest::testProperties()
{
    Library lib;
    QVERIFY(!lib.isValid());

    QSignalSpy nameChanged(&lib, &Library::nameChanged);

    lib.setName("My Library");

    QCOMPARE(nameChanged.count(), 1);

    QCOMPARE(lib.name(), QString("My Library"));
}

void LibraryTest::testLoad()
{
    Library lib(m_dir->path());
    lib.save();

    Library lib2(m_dir->path());
    QVERIFY(lib2.load());
    QCOMPARE(lib2.name(), lib.name());
}

void LibraryTest::testFromJson()
{
    Library lib;
    lib.fromJson("{"
                 "\"name\": \"foo\","
                 "\"uid\": \"{6ca12b27-fc18-4257-9460-ef8dfed622bb}\""
                 "}");
    QCOMPARE(lib.uid(), QUuid("{6ca12b27-fc18-4257-9460-ef8dfed622bb}"));
    QCOMPARE(lib.name(), QString("foo"));
}

void LibraryTest::testEncache()
{
    Library lib(m_dir->path());
    lib.setName("Foo");
    auto cacheEntry = lib.encache();
    QVERIFY(cacheEntry.valid);
    QCOMPARE(cacheEntry.id, lib.uid());
}

void LibraryTest::testDecache()
{
    Library lib(m_dir->path());
    lib.setName("Foo");
    auto cacheEntry = lib.encache();

    QScopedPointer<Library> lib2(Library::decache(cacheEntry));
    QCOMPARE(lib2->uid(), lib.uid());
    QCOMPARE(lib2->name(), lib.name());
    QCOMPARE(lib2->directory(), lib.directory());
}

void LibraryTest::testVariant()
{
    Library lib1;
    lib1.setName("Foo");
    Library lib2;
    lib2.fromVariant(lib1.toVariant());
    QCOMPARE(lib2.name(), lib1.name());
    QCOMPARE(lib2.uid(), lib1.uid());
}

void LibraryTest::cleanup()
{
    delete m_dir;
}

QTEST_MAIN(LibraryTest)
#include "test_library.moc"
