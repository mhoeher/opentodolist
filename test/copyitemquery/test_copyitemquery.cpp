/*
 * Copyright 2021 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDir>
#include <QObject>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "datamodel/image.h"
#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/notepage.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"

#include "datastorage/cache.h"
#include "datastorage/copyitemquery.h"
#include "datastorage/getitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"

class CopyItemQueryTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init();
    void copyImageSameLibrary();
    void copyNoteSameLibrary();
    void copyTodoListSameLibrary();
    void copyTodoSameLibrary();
    void copyTaskSameLibrary();
    void copyImageDifferentLibrary();
    void copyNoteDifferentLibrary();
    void copyTodoListDifferentLibrary();
    void copyTodoDifferentLibrary();
    void copyTaskDifferentLibrary();
    void cleanup();
    void cleanupTestCase() {}

private:
    QTemporaryDir* cacheDir;
    Cache* cache;

    static bool differentFilesHaveEqualContent(const QString& filename1, const QString& filename2);
};

void CopyItemQueryTest::init()
{
    cacheDir = new QTemporaryDir();
    cache = new Cache();
    cache->setCacheDirectory(cacheDir->path());
    cache->setCacheSize(1024 * 1024);
    QVERIFY(cache->open());
}

void CopyItemQueryTest::copyImageSameLibrary()
{
    QTemporaryDir tmpDir1;

    Library lib1(tmpDir1.path());
    QDir dir(lib1.newItemLocation());
    QVERIFY(dir.mkpath("."));
    Image img1(dir);
    img1.setLibraryId(lib1.uid());
    img1.setImage(SAMPLES_PATH "/image.jpg");
    img1.setTags({ "Foo", "Bar" });
    img1.setColor(TopLevelItem::Blue);
    img1.setTitle("Look, a nice flower!");
    img1.setDueTo(QDateTime::currentDateTime());
    img1.setNotes("This photo shows a yellow flower.");
    img1.setRecurInterval(5);
    img1.setRecurrencePattern(ComplexItem::RecurEveryNDays);
    img1.setRecurrenceSchedule(ComplexItem::RelativeToCurrentDate);
    img1.markCurrentOccurrenceAsDone();
    img1.attachFile(SAMPLES_PATH "/test1.txt");

    {
        auto q = new InsertOrUpdateItemsQuery;
        q->add(&lib1);
        q->add(&img1);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new CopyItemQuery;
        q->copyItem(&img1, &lib1);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new GetItemsQuery;
        q->setParent(lib1.uid());
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 2);
        auto img2 = qSharedPointerCast<Image>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        if (img2->uid() == img1.uid()) {
            img2 = qSharedPointerCast<Image>(
                    QSharedPointer<Item>(Item::decache(items.at(1).value<ItemCacheEntry>())));
        }
        // The two should be - attribute wise - identical.
        QVERIFY(img1.uid() != img2->uid());
        QVERIFY(differentFilesHaveEqualContent(img2->imageUrl().toLocalFile(),
                                               img1.imageUrl().toLocalFile()));
        QCOMPARE(img2->libraryId(), img1.libraryId());
        QCOMPARE(img2->tags(), img1.tags());
        QCOMPARE(img2->color(), img1.color());
        QCOMPARE(img2->title(), "Copy of " + img1.title());
        QCOMPARE(img2->dueTo(), img1.dueTo());
        QCOMPARE(img2->recurInterval(), img1.recurInterval());
        QCOMPARE(img2->recurrencePattern(), img1.recurrencePattern());
        QCOMPARE(img2->recurrenceSchedule(), img1.recurrenceSchedule());
        QCOMPARE(img2->effectiveDueTo(), img1.effectiveDueTo());
        QCOMPARE(img2->attachments().length(), 1);
        QVERIFY(differentFilesHaveEqualContent(img2->attachmentFileName(img2->attachments().at(0)),
                                               img1.attachmentFileName(img1.attachments().at(0))));
    }
}

void CopyItemQueryTest::copyNoteSameLibrary()
{
    QTemporaryDir tmpDir1;

    Library lib1(tmpDir1.path());
    QDir dir(lib1.newItemLocation());
    QVERIFY(dir.mkpath("."));
    Note note1(dir);
    note1.setLibraryId(lib1.uid());
    note1.setTags({ "Foo", "Bar" });
    note1.setColor(TopLevelItem::Blue);
    note1.setTitle("This is a note");
    note1.setDueTo(QDateTime::currentDateTime());
    note1.setNotes("And this it's text.");
    NotePage page1(dir);
    page1.setTitle("A page");
    page1.setNotes("Page text");
    page1.setNoteUid(note1.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        q->add(&lib1);
        q->add(&note1);
        q->add(&page1);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new CopyItemQuery;
        q->copyItem(&note1, &lib1);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    QUuid note2Uid;

    {
        auto q = new GetItemsQuery;
        q->setParent(lib1.uid());
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 2);
        auto note2 = qSharedPointerCast<Image>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        if (note2->uid() == note1.uid()) {
            note2 = qSharedPointerCast<Image>(
                    QSharedPointer<Item>(Item::decache(items.at(1).value<ItemCacheEntry>())));
        }
        // The two should be - attribute wise - identical.
        QVERIFY(note1.uid() != note2->uid());
        QCOMPARE(note2->libraryId(), note1.libraryId());
        QCOMPARE(note2->tags(), note1.tags());
        QCOMPARE(note2->color(), note1.color());
        QCOMPARE(note2->title(), "Copy of " + note1.title());
        QCOMPARE(note2->dueTo(), note1.dueTo());
        QCOMPARE(note2->recurInterval(), note1.recurInterval());
        QCOMPARE(note2->recurrencePattern(), note1.recurrencePattern());
        QCOMPARE(note2->recurrenceSchedule(), note1.recurrenceSchedule());
        QCOMPARE(note2->effectiveDueTo(), note1.effectiveDueTo());
        note2Uid = note2->uid();
    }

    {
        auto q = new GetItemsQuery;
        q->setParent(note2Uid);
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto page2 = qSharedPointerCast<NotePage>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(page2->uid() != page1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(page1.uid() != page2->uid());
        QVERIFY(page2->noteUid() != page1.noteUid());
        QCOMPARE(page2->title(), page1.title());
        QCOMPARE(page2->dueTo(), page1.dueTo());
        QCOMPARE(page2->recurInterval(), page1.recurInterval());
        QCOMPARE(page2->recurrencePattern(), page1.recurrencePattern());
        QCOMPARE(page2->recurrenceSchedule(), page1.recurrenceSchedule());
        QCOMPARE(page2->effectiveDueTo(), page1.effectiveDueTo());
        note2Uid = page2->uid();
    }
}

void CopyItemQueryTest::copyTodoListSameLibrary()
{
    QTemporaryDir tmpDir1;

    Library lib1(tmpDir1.path());
    QDir dir(lib1.newItemLocation());
    QVERIFY(dir.mkpath("."));
    TodoList todoList1(dir);
    todoList1.setLibraryId(lib1.uid());
    todoList1.setTags({ "Foo", "Bar" });
    todoList1.setColor(TopLevelItem::Blue);
    todoList1.setTitle("A todo list!");
    todoList1.setDueTo(QDateTime::currentDateTime());
    todoList1.setNotes("Some descriptive text");
    Todo todo1(dir);
    todo1.setTitle("A todo");
    todo1.setNotes("... with some text.");
    todo1.setTodoListUid(todoList1.uid());
    todo1.attachFile(SAMPLES_PATH "/test2.txt");
    Task task1(dir);
    task1.setTitle("A task");
    task1.setDone(true);
    task1.setTodoUid(todo1.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        q->add(&lib1);
        q->add(&todoList1);
        q->add(&todo1);
        q->add(&task1);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new CopyItemQuery;
        q->copyItem(&todoList1, &lib1);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    QUuid todoList2Uid;

    {
        auto q = new GetItemsQuery;
        q->setParent(lib1.uid());
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 2);
        auto todoList2 = qSharedPointerCast<TodoList>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        if (todoList2->uid() == todoList1.uid()) {
            todoList2 = qSharedPointerCast<TodoList>(
                    QSharedPointer<Item>(Item::decache(items.at(1).value<ItemCacheEntry>())));
        }
        // The two should be - attribute wise - identical.
        QVERIFY(todoList1.uid() != todoList2->uid());
        QCOMPARE(todoList2->libraryId(), todoList1.libraryId());
        QCOMPARE(todoList2->tags(), todoList1.tags());
        QCOMPARE(todoList2->color(), todoList1.color());
        QCOMPARE(todoList2->title(), "Copy of " + todoList1.title());
        QCOMPARE(todoList2->dueTo(), todoList1.dueTo());
        QCOMPARE(todoList2->recurInterval(), todoList1.recurInterval());
        QCOMPARE(todoList2->recurrencePattern(), todoList1.recurrencePattern());
        QCOMPARE(todoList2->recurrenceSchedule(), todoList1.recurrenceSchedule());
        QCOMPARE(todoList2->effectiveDueTo(), todoList1.effectiveDueTo());
        todoList2Uid = todoList2->uid();
    }

    QUuid todo2Uid;

    {
        auto q = new GetItemsQuery;
        q->setParent(todoList2Uid);
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto todo2 = qSharedPointerCast<Todo>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(todo2->uid() != todo1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(todo1.uid() != todo2->uid());
        QVERIFY(todo2->todoListUid() != todo1.todoListUid());
        QCOMPARE(todo2->title(), todo1.title());
        QCOMPARE(todo2->done(), todo1.done());
        QCOMPARE(todo2->dueTo(), todo1.dueTo());
        QCOMPARE(todo2->recurInterval(), todo1.recurInterval());
        QCOMPARE(todo2->recurrencePattern(), todo1.recurrencePattern());
        QCOMPARE(todo2->recurrenceSchedule(), todo1.recurrenceSchedule());
        QCOMPARE(todo2->effectiveDueTo(), todo1.effectiveDueTo());
        QCOMPARE(todo2->attachments().length(), 1);
        QVERIFY(differentFilesHaveEqualContent(
                todo2->attachmentFileName(todo2->attachments().at(0)),
                todo1.attachmentFileName(todo1.attachments().at(0))));
        todo2Uid = todo2->uid();
    }

    {
        auto q = new GetItemsQuery;
        q->setParent(todo2Uid);
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto task2 = qSharedPointerCast<Task>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(task2->uid() != todo1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(task1.uid() != task2->uid());
        QVERIFY(task2->todoUid() != task1.todoUid());
        QCOMPARE(task2->title(), task1.title());
        QCOMPARE(task2->done(), task1.done());
    }
}

void CopyItemQueryTest::copyTodoSameLibrary()
{
    QTemporaryDir tmpDir1;

    Library lib1(tmpDir1.path());
    QDir dir(lib1.newItemLocation());

    QVERIFY(dir.mkpath("."));
    TodoList todoList1(dir);
    todoList1.setLibraryId(lib1.uid());
    todoList1.setTags({ "Foo", "Bar" });
    todoList1.setColor(TopLevelItem::Blue);
    todoList1.setTitle("A todo list!");
    todoList1.setDueTo(QDateTime::currentDateTime());
    todoList1.setNotes("Some descriptive text");
    Todo todo1(dir);
    todo1.setTitle("A todo");
    todo1.setNotes("... with some text.");
    todo1.setTodoListUid(todoList1.uid());
    todo1.attachFile(SAMPLES_PATH "/test2.txt");
    Task task1(dir);
    task1.setTitle("A task");
    task1.setDone(true);
    task1.setTodoUid(todo1.uid());
    TodoList todoList2(dir);
    todoList2.setLibraryId(lib1.uid());
    todoList2.setColor(TopLevelItem::Green);
    todoList2.setTitle("Another todo list!");
    todoList2.setDueTo(QDateTime::currentDateTime());
    todoList2.setNotes("Some descriptive text");

    {
        auto q = new InsertOrUpdateItemsQuery;
        q->add(&lib1);
        q->add(&todoList1);
        q->add(&todo1);
        q->add(&task1);
        q->add(&todoList2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new CopyItemQuery;
        q->copyItem(&todo1, &lib1, &todoList2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    QUuid todo2Uid;

    {
        auto q = new GetItemsQuery;
        q->setParent(todoList2.uid());
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto todo2 = qSharedPointerCast<Todo>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(todo2->uid() != todo1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(todo1.uid() != todo2->uid());
        QVERIFY(todo2->todoListUid() != todo1.todoListUid());
        QCOMPARE(todo2->title(), todo1.title());
        QCOMPARE(todo2->done(), todo1.done());
        QCOMPARE(todo2->dueTo(), todo1.dueTo());
        QCOMPARE(todo2->recurInterval(), todo1.recurInterval());
        QCOMPARE(todo2->recurrencePattern(), todo1.recurrencePattern());
        QCOMPARE(todo2->recurrenceSchedule(), todo1.recurrenceSchedule());
        QCOMPARE(todo2->effectiveDueTo(), todo1.effectiveDueTo());
        QCOMPARE(todo2->attachments().length(), 1);
        QVERIFY(differentFilesHaveEqualContent(
                todo2->attachmentFileName(todo2->attachments().at(0)),
                todo1.attachmentFileName(todo1.attachments().at(0))));
        todo2Uid = todo2->uid();
    }

    {
        auto q = new GetItemsQuery;
        q->setParent(todo2Uid);
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto task2 = qSharedPointerCast<Task>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(task2->uid() != todo1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(task1.uid() != task2->uid());
        QVERIFY(task2->todoUid() != task1.todoUid());
        QCOMPARE(task2->title(), task1.title());
        QCOMPARE(task2->done(), task1.done());
    }
}

void CopyItemQueryTest::copyTaskSameLibrary()
{
    QTemporaryDir tmpDir1;

    Library lib1(tmpDir1.path());
    QDir dir(lib1.newItemLocation());

    QVERIFY(dir.mkpath("."));
    TodoList todoList1(dir);
    todoList1.setLibraryId(lib1.uid());
    todoList1.setTags({ "Foo", "Bar" });
    todoList1.setColor(TopLevelItem::Blue);
    todoList1.setTitle("A todo list!");
    todoList1.setDueTo(QDateTime::currentDateTime());
    todoList1.setNotes("Some descriptive text");
    Todo todo1(dir);
    todo1.setTitle("A todo");
    todo1.setNotes("... with some text.");
    todo1.setTodoListUid(todoList1.uid());
    todo1.attachFile(SAMPLES_PATH "/test2.txt");
    Task task1(dir);
    task1.setTitle("A task");
    task1.setDone(true);
    task1.setTodoUid(todo1.uid());
    TodoList todoList2(dir);
    todoList2.setLibraryId(lib1.uid());
    todoList2.setColor(TopLevelItem::Green);
    todoList2.setTitle("Another todo list!");
    todoList2.setDueTo(QDateTime::currentDateTime());
    todoList2.setNotes("Some descriptive text");
    Todo todo2(dir);
    todo2.setTitle("Another todo");
    todo2.setNotes("... with some other text.");
    todo2.setTodoListUid(todoList2.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        q->add(&lib1);
        q->add(&todoList1);
        q->add(&todo1);
        q->add(&task1);
        q->add(&todoList2);
        q->add(&todo2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new CopyItemQuery;
        q->copyItem(&task1, &lib1, &todo2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new GetItemsQuery;
        q->setParent(todo2.uid());
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto task2 = qSharedPointerCast<Task>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(task2->uid() != todo1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(task1.uid() != task2->uid());
        QVERIFY(task2->todoUid() != task1.todoUid());
        QCOMPARE(task2->title(), task1.title());
        QCOMPARE(task2->done(), task1.done());
    }
}

void CopyItemQueryTest::copyImageDifferentLibrary()
{
    QTemporaryDir tmpDir1;
    QTemporaryDir tmpDir2;

    Library lib1(tmpDir1.path());
    Library lib2(tmpDir2.path());
    QDir dir(lib1.newItemLocation());
    QVERIFY(dir.mkpath("."));
    Image img1(dir);
    img1.setLibraryId(lib1.uid());
    img1.setImage(SAMPLES_PATH "/image.jpg");
    img1.setTags({ "Foo", "Bar" });
    img1.setColor(TopLevelItem::Blue);
    img1.setTitle("Look, a nice flower!");
    img1.setDueTo(QDateTime::currentDateTime());
    img1.setNotes("This photo shows a yellow flower.");
    img1.setRecurInterval(5);
    img1.setRecurrencePattern(ComplexItem::RecurEveryNDays);
    img1.setRecurrenceSchedule(ComplexItem::RelativeToCurrentDate);
    img1.markCurrentOccurrenceAsDone();
    img1.attachFile(SAMPLES_PATH "/test1.txt");

    {
        auto q = new InsertOrUpdateItemsQuery;
        q->add(&lib1);
        q->add(&lib2);
        q->add(&img1);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new CopyItemQuery;
        q->copyItem(&img1, &lib2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new GetItemsQuery;
        q->setParent(lib2.uid());
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image from the new library:
        QCOMPARE(items.length(), 1);
        auto img2 = qSharedPointerCast<Image>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QCOMPARE(img2->libraryId(), lib2.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(img1.uid() != img2->uid());
        QVERIFY(differentFilesHaveEqualContent(img2->imageUrl().toLocalFile(),
                                               img1.imageUrl().toLocalFile()));
        QCOMPARE(img2->libraryId(), lib2.uid());
        QCOMPARE(img2->tags(), img1.tags());
        QCOMPARE(img2->color(), img1.color());
        QCOMPARE(img2->title(), img1.title());
        QCOMPARE(img2->dueTo(), img1.dueTo());
        QCOMPARE(img2->recurInterval(), img1.recurInterval());
        QCOMPARE(img2->recurrencePattern(), img1.recurrencePattern());
        QCOMPARE(img2->recurrenceSchedule(), img1.recurrenceSchedule());
        QCOMPARE(img2->effectiveDueTo(), img1.effectiveDueTo());
        QCOMPARE(img2->attachments().length(), 1);
        QVERIFY(differentFilesHaveEqualContent(img2->attachmentFileName(img2->attachments().at(0)),
                                               img1.attachmentFileName(img1.attachments().at(0))));
    }
}

void CopyItemQueryTest::copyNoteDifferentLibrary()
{
    QTemporaryDir tmpDir1;
    QTemporaryDir tmpDir2;

    Library lib1(tmpDir1.path());
    Library lib2(tmpDir2.path());
    QDir dir(lib1.newItemLocation());
    QVERIFY(dir.mkpath("."));
    Note note1(dir);
    note1.setLibraryId(lib1.uid());
    note1.setTags({ "Foo", "Bar" });
    note1.setColor(TopLevelItem::Blue);
    note1.setTitle("This is a note");
    note1.setDueTo(QDateTime::currentDateTime());
    note1.setNotes("And this it's text.");
    NotePage page1(dir);
    page1.setTitle("A page");
    page1.setNotes("Page text");
    page1.setNoteUid(note1.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        q->add(&lib1);
        q->add(&lib2);
        q->add(&note1);
        q->add(&page1);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new CopyItemQuery;
        q->copyItem(&note1, &lib2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    QUuid note2Uid;

    {
        auto q = new GetItemsQuery;
        q->setParent(lib2.uid());
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the note in the new library:
        QCOMPARE(items.length(), 1);
        auto note2 = qSharedPointerCast<Note>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QCOMPARE(note2->libraryId(), lib2.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(note1.uid() != note2->uid());
        QCOMPARE(note2->libraryId(), lib2.uid());
        QCOMPARE(note2->tags(), note1.tags());
        QCOMPARE(note2->color(), note1.color());
        QCOMPARE(note2->title(), note1.title());
        QCOMPARE(note2->dueTo(), note1.dueTo());
        QCOMPARE(note2->recurInterval(), note1.recurInterval());
        QCOMPARE(note2->recurrencePattern(), note1.recurrencePattern());
        QCOMPARE(note2->recurrenceSchedule(), note1.recurrenceSchedule());
        QCOMPARE(note2->effectiveDueTo(), note1.effectiveDueTo());
        note2Uid = note2->uid();
    }

    {
        auto q = new GetItemsQuery;
        q->setParent(note2Uid);
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto page2 = qSharedPointerCast<NotePage>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(page2->uid() != page1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(page1.uid() != page2->uid());
        QVERIFY(page2->noteUid() != page1.noteUid());
        QCOMPARE(page2->title(), page1.title());
        QCOMPARE(page2->dueTo(), page1.dueTo());
        QCOMPARE(page2->recurInterval(), page1.recurInterval());
        QCOMPARE(page2->recurrencePattern(), page1.recurrencePattern());
        QCOMPARE(page2->recurrenceSchedule(), page1.recurrenceSchedule());
        QCOMPARE(page2->effectiveDueTo(), page1.effectiveDueTo());
        note2Uid = page2->uid();
    }
}

void CopyItemQueryTest::copyTodoListDifferentLibrary()
{
    QTemporaryDir tmpDir1;
    QTemporaryDir tmpDir2;

    Library lib1(tmpDir1.path());
    Library lib2(tmpDir2.path());
    QDir dir(lib1.newItemLocation());
    QVERIFY(dir.mkpath("."));
    TodoList todoList1(dir);
    todoList1.setLibraryId(lib1.uid());
    todoList1.setTags({ "Foo", "Bar" });
    todoList1.setColor(TopLevelItem::Blue);
    todoList1.setTitle("A todo list!");
    todoList1.setDueTo(QDateTime::currentDateTime());
    todoList1.setNotes("Some descriptive text");
    Todo todo1(dir);
    todo1.setTitle("A todo");
    todo1.setNotes("... with some text.");
    todo1.setTodoListUid(todoList1.uid());
    todo1.attachFile(SAMPLES_PATH "/test2.txt");
    Task task1(dir);
    task1.setTitle("A task");
    task1.setDone(true);
    task1.setTodoUid(todo1.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        q->add(&lib1);
        q->add(&lib2);
        q->add(&todoList1);
        q->add(&todo1);
        q->add(&task1);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new CopyItemQuery;
        q->copyItem(&todoList1, &lib2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    QUuid todoList2Uid;

    {
        auto q = new GetItemsQuery;
        q->setParent(lib2.uid());
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto todoList2 = qSharedPointerCast<TodoList>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        // The two should be - attribute wise - identical.
        QVERIFY(todoList1.uid() != todoList2->uid());
        QCOMPARE(todoList2->libraryId(), lib2.uid());
        QCOMPARE(todoList2->tags(), todoList1.tags());
        QCOMPARE(todoList2->color(), todoList1.color());
        QCOMPARE(todoList2->title(), todoList1.title());
        QCOMPARE(todoList2->dueTo(), todoList1.dueTo());
        QCOMPARE(todoList2->recurInterval(), todoList1.recurInterval());
        QCOMPARE(todoList2->recurrencePattern(), todoList1.recurrencePattern());
        QCOMPARE(todoList2->recurrenceSchedule(), todoList1.recurrenceSchedule());
        QCOMPARE(todoList2->effectiveDueTo(), todoList1.effectiveDueTo());
        todoList2Uid = todoList2->uid();
    }

    QUuid todo2Uid;

    {
        auto q = new GetItemsQuery;
        q->setParent(todoList2Uid);
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto todo2 = qSharedPointerCast<Todo>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(todo2->uid() != todo1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(todo1.uid() != todo2->uid());
        QVERIFY(todo2->todoListUid() != todo1.todoListUid());
        QCOMPARE(todo2->title(), todo1.title());
        QCOMPARE(todo2->done(), todo1.done());
        QCOMPARE(todo2->dueTo(), todo1.dueTo());
        QCOMPARE(todo2->recurInterval(), todo1.recurInterval());
        QCOMPARE(todo2->recurrencePattern(), todo1.recurrencePattern());
        QCOMPARE(todo2->recurrenceSchedule(), todo1.recurrenceSchedule());
        QCOMPARE(todo2->effectiveDueTo(), todo1.effectiveDueTo());
        todo2Uid = todo2->uid();
    }

    {
        auto q = new GetItemsQuery;
        q->setParent(todo2Uid);
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto task2 = qSharedPointerCast<Task>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(task2->uid() != todo1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(task1.uid() != task2->uid());
        QVERIFY(task2->todoUid() != task1.todoUid());
        QCOMPARE(task2->title(), task1.title());
        QCOMPARE(task2->done(), task1.done());
    }
}

void CopyItemQueryTest::copyTodoDifferentLibrary()
{
    QTemporaryDir tmpDir1;
    QTemporaryDir tmpDir2;

    Library lib1(tmpDir1.path());
    Library lib2(tmpDir2.path());
    QDir dir1(lib1.newItemLocation());
    QDir dir2(lib2.newItemLocation());

    QVERIFY(dir1.mkpath("."));
    QVERIFY(dir2.mkpath("."));
    TodoList todoList1(dir1);
    todoList1.setLibraryId(lib1.uid());
    todoList1.setTags({ "Foo", "Bar" });
    todoList1.setColor(TopLevelItem::Blue);
    todoList1.setTitle("A todo list!");
    todoList1.setDueTo(QDateTime::currentDateTime());
    todoList1.setNotes("Some descriptive text");
    Todo todo1(dir1);
    todo1.setTitle("A todo");
    todo1.setNotes("... with some text.");
    todo1.setTodoListUid(todoList1.uid());
    todo1.attachFile(SAMPLES_PATH "/test2.txt");
    Task task1(dir1);
    task1.setTitle("A task");
    task1.setDone(true);
    task1.setTodoUid(todo1.uid());
    TodoList todoList2(dir2);
    todoList2.setLibraryId(lib1.uid());
    todoList2.setColor(TopLevelItem::Green);
    todoList2.setTitle("Another todo list!");
    todoList2.setDueTo(QDateTime::currentDateTime());
    todoList2.setNotes("Some descriptive text");

    {
        auto q = new InsertOrUpdateItemsQuery;
        q->add(&lib1);
        q->add(&lib2);
        q->add(&todoList1);
        q->add(&todo1);
        q->add(&task1);
        q->add(&todoList2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new CopyItemQuery;
        q->copyItem(&todo1, &lib1, &todoList2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    QUuid todo2Uid;

    {
        auto q = new GetItemsQuery;
        q->setParent(todoList2.uid());
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto todo2 = qSharedPointerCast<Todo>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(todo2->uid() != todo1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(todo1.uid() != todo2->uid());
        QVERIFY(todo2->todoListUid() != todo1.todoListUid());
        QCOMPARE(todo2->title(), todo1.title());
        QCOMPARE(todo2->done(), todo1.done());
        QCOMPARE(todo2->dueTo(), todo1.dueTo());
        QCOMPARE(todo2->recurInterval(), todo1.recurInterval());
        QCOMPARE(todo2->recurrencePattern(), todo1.recurrencePattern());
        QCOMPARE(todo2->recurrenceSchedule(), todo1.recurrenceSchedule());
        QCOMPARE(todo2->effectiveDueTo(), todo1.effectiveDueTo());
        QCOMPARE(todo2->attachments().length(), 1);
        QVERIFY(differentFilesHaveEqualContent(
                todo2->attachmentFileName(todo2->attachments().at(0)),
                todo1.attachmentFileName(todo1.attachments().at(0))));
        todo2Uid = todo2->uid();
    }

    {
        auto q = new GetItemsQuery;
        q->setParent(todo2Uid);
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto task2 = qSharedPointerCast<Task>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(task2->uid() != todo1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(task1.uid() != task2->uid());
        QVERIFY(task2->todoUid() != task1.todoUid());
        QCOMPARE(task2->title(), task1.title());
        QCOMPARE(task2->done(), task1.done());
    }
}

void CopyItemQueryTest::copyTaskDifferentLibrary()
{
    QTemporaryDir tmpDir1;
    QTemporaryDir tmpDir2;

    Library lib1(tmpDir1.path());
    Library lib2(tmpDir2.path());
    QDir dir1(lib1.newItemLocation());
    QDir dir2(lib2.newItemLocation());

    QVERIFY(dir1.mkpath("."));
    QVERIFY(dir2.mkpath("."));
    TodoList todoList1(dir1);
    todoList1.setLibraryId(lib1.uid());
    todoList1.setTags({ "Foo", "Bar" });
    todoList1.setColor(TopLevelItem::Blue);
    todoList1.setTitle("A todo list!");
    todoList1.setDueTo(QDateTime::currentDateTime());
    todoList1.setNotes("Some descriptive text");
    Todo todo1(dir1);
    todo1.setTitle("A todo");
    todo1.setNotes("... with some text.");
    todo1.setTodoListUid(todoList1.uid());
    todo1.attachFile(SAMPLES_PATH "/test2.txt");
    Task task1(dir1);
    task1.setTitle("A task");
    task1.setDone(true);
    task1.setTodoUid(todo1.uid());
    TodoList todoList2(dir2);
    todoList2.setLibraryId(lib1.uid());
    todoList2.setColor(TopLevelItem::Green);
    todoList2.setTitle("Another todo list!");
    todoList2.setDueTo(QDateTime::currentDateTime());
    todoList2.setNotes("Some descriptive text");
    Todo todo2(dir2);
    todo2.setTitle("Another todo");
    todo2.setNotes("... with some other text.");
    todo2.setTodoListUid(todoList2.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        q->add(&lib1);
        q->add(&lib2);
        q->add(&todoList1);
        q->add(&todo1);
        q->add(&task1);
        q->add(&todoList2);
        q->add(&todo2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new CopyItemQuery;
        q->copyItem(&task1, &lib1, &todo2);
        cache->run(q);
        QSignalSpy finished(q, &ItemsQuery::finished);
        QVERIFY(finished.wait());
    }

    {
        auto q = new GetItemsQuery;
        q->setParent(todo2.uid());
        cache->run(q);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QVERIFY(itemsAvailable.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        // We should have the image and its copy:
        QCOMPARE(items.length(), 1);
        auto task2 = qSharedPointerCast<Task>(
                QSharedPointer<Item>(Item::decache(items.at(0).value<ItemCacheEntry>())));
        QVERIFY(task2->uid() != todo1.uid());
        // The two should be - attribute wise - identical.
        QVERIFY(task1.uid() != task2->uid());
        QVERIFY(task2->todoUid() != task1.todoUid());
        QCOMPARE(task2->title(), task1.title());
        QCOMPARE(task2->done(), task1.done());
    }
}

void CopyItemQueryTest::cleanup()
{
    delete cache;
    delete cacheDir;
}

/**
 * @brief Utility method to compare files.
 *
 * This method compares the two files and returned true iff the paths are different but the content
 * is byte-wise identical.
 */
bool CopyItemQueryTest::differentFilesHaveEqualContent(const QString& filename1,
                                                       const QString& filename2)
{
    if (filename1 == filename2) {
        return false;
    }
    QFile file1(filename1);
    if (file1.open(QIODevice::ReadOnly)) {
        QFile file2(filename2);
        if (file2.open(QIODevice::ReadOnly)) {
            return file1.readAll() == file2.readAll();
        }
    }
    return false;
}

QTEST_MAIN(CopyItemQueryTest)
#include "test_copyitemquery.moc"
