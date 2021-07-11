/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
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

#include <QTemporaryDir>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "datamodel/complexitem.h"

class ComplexItemTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void testProperties();
    void testPersistence();
    void attachments();
    void recurrence();
    void cleanupTestCase() {}
};

void ComplexItemTest::testProperties()
{
    ComplexItem item;
    QSignalSpy dueToChanged(&item, &ComplexItem::dueToChanged);
    QSignalSpy notesChanged(&item, &ComplexItem::notesChanged);

    item.setDueTo(QDateTime::currentDateTime());
    item.setNotes("Hello World");

    QCOMPARE(dueToChanged.count(), 1);
    QCOMPARE(notesChanged.count(), 1);
}

void ComplexItemTest::testPersistence()
{
    ComplexItem item, anotherItem;
    QSignalSpy dueToChanged(&anotherItem, &ComplexItem::dueToChanged);
    QSignalSpy notesChanged(&anotherItem, &ComplexItem::notesChanged);

    item.setDueTo(QDateTime::currentDateTime());
    item.setNotes("Hello World");

    anotherItem.fromVariant(item.toVariant());

    QCOMPARE(dueToChanged.count(), 1);
    QCOMPARE(notesChanged.count(), 1);

    QCOMPARE(item.dueTo(), anotherItem.dueTo());
    QCOMPARE(item.notes(), anotherItem.notes());
}

void ComplexItemTest::attachments()
{
    QTemporaryDir dir;
    QDir d(dir.path());
    d.mkdir("item");
    d.mkdir("res");
    ComplexItem item(d.absoluteFilePath("item/item.otl"));
    QSignalSpy attachmentsChanged(&item, &ComplexItem::attachmentsChanged);
    QFile file(d.absoluteFilePath("res/foo.txt"));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("Hello World");
    file.close();

    item.attachFile(d.absoluteFilePath("res/foo.txt"));
    QCOMPARE(attachmentsChanged.count(), 1);
    QCOMPARE(item.attachments(), QStringList({ "foo.txt" }));
    QVERIFY(QFile(item.attachmentFileName("foo.txt")).exists());

    item.attachFile(d.absoluteFilePath("res/foo.txt"));
    QCOMPARE(attachmentsChanged.count(), 2);
    QCOMPARE(item.attachments(), QStringList({ "foo-1.txt", "foo.txt" }));
    QVERIFY(QFile(item.attachmentFileName("foo-1.txt")).exists());

    item.detachFile("foo.txt");
    QCOMPARE(attachmentsChanged.count(), 3);
    QCOMPARE(item.attachments(), QStringList({ "foo-1.txt" }));
    QVERIFY(QFile(item.attachmentFileName("foo-1.txt")).exists());
    QVERIFY(!QFile(item.attachmentFileName("foo.txt")).exists());
    item.save();

    auto attachmentPath = item.attachmentFileName("foo-1.txt");
    QVERIFY(QFile(attachmentPath).exists());
    QVERIFY(item.deleteItem());
    QVERIFY(!QFile(attachmentPath).exists());
}

/**
 * @brief Excercise the recurrence functionality of items.
 */
void ComplexItemTest::recurrence()
{
    {
        // By default, recurrence should be off with same defaults:
        ComplexItem item;
        QCOMPARE(item.recurrencePattern(), ComplexItem::NoRecurrence);
        QCOMPARE(item.recurrenceSchedule(), ComplexItem::RelativeToOriginalDueDate);
        QCOMPARE(item.recurInterval(), 0);
        QVERIFY(item.nextDueTo().isNull());
    }

    {
        // Recurrence data shall be saved and restored properly:
        ComplexItem item;
        item.setRecurrencePattern(ComplexItem::RecurEveryNDays);
        item.setRecurInterval(14);
        item.setRecurrenceSchedule(ComplexItem::RelativeToCurrentDate);
        item.setNextDueTo(QDateTime::currentDateTime());

        ComplexItem item2;
        item2.fromVariant(item.toVariant());
        QCOMPARE(item2.recurrencePattern(), ComplexItem::RecurEveryNDays);
        QCOMPARE(item2.recurrenceSchedule(), ComplexItem::RelativeToCurrentDate);
        QCOMPARE(item2.recurInterval(), 14);
        QCOMPARE(item2.nextDueTo(), item.nextDueTo());
    }

    {
        // Marking an item without a recurrence pattern set as done should just remove the due date:
        ComplexItem item;
        item.setDueTo(QDateTime::currentDateTime());
        item.markCurrentOccurrenceAsDone();
        QVERIFY(item.dueTo().isNull());
    }

    {
        // Marking an item which recurs daily as done should reschedule it to tomorrow.
        ComplexItem item;
        auto today = QDate::currentDate().startOfDay();
        item.setDueTo(today);
        item.setRecurrencePattern(ComplexItem::RecurDaily);
        QCOMPARE(item.dueTo(), item.effectiveDueTo());
        QSignalSpy nextDueToChanged(&item, &ComplexItem::nextDueToChanged);
        QSignalSpy effectiveDueToChanged(&item, &ComplexItem::effectiveDueToChanged);
        item.markCurrentOccurrenceAsDone(today);
        QCOMPARE(nextDueToChanged.count(), 1);
        QCOMPARE(effectiveDueToChanged.count(), 1);
        auto expectedNextDate = today.addDays(1);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today);

        // This is also the case, when we mark an item as done which is overdue:
        item.setDueTo(today.addDays(-14)); // Ups, was done 2 weeks ago
        item.markCurrentOccurrenceAsDone(today);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today.addDays(-14));

        // If we mark an item which is in the future as done, schedule it one day further:
        item.setDueTo(today.addDays(1));
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = today.addDays(2);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today.addDays(1));

        // For daily recurrence, the scheduling offset does not matter:
        item.setDueTo(today.addDays(-1));
        item.setRecurrenceSchedule(ComplexItem::RelativeToCurrentDate);
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = today.addDays(1);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today.addDays(-1));
    }

    {
        // Marking an item which recurs week should re-schedule to the following week
        ComplexItem item;
        auto today = QDate::currentDate().startOfDay();
        item.setDueTo(today);
        item.setRecurrencePattern(ComplexItem::RecurWeekly);
        QCOMPARE(item.dueTo(), item.effectiveDueTo());
        QSignalSpy nextDueToChanged(&item, &ComplexItem::nextDueToChanged);
        QSignalSpy effectiveDueToChanged(&item, &ComplexItem::effectiveDueToChanged);
        item.markCurrentOccurrenceAsDone(today);
        QCOMPARE(nextDueToChanged.count(), 1);
        QCOMPARE(effectiveDueToChanged.count(), 1);
        auto expectedNextDate = today.addDays(7);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today);

        // If the item is overdue, the standard scheduling should still just schedule a few days
        // into the future:
        item.setDueTo(today.addDays(-3));
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = today.addDays(4);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today.addDays(-3));

        // If the item is still in the future, schedule one additional week into the future:
        item.setDueTo(today.addDays(3));
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = today.addDays(10);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today.addDays(3));

        // Using scheduling relative to today, the next occurrence should be one week in the future
        // from now:
        item.setDueTo(today.addDays(-4));
        item.setRecurrenceSchedule(ComplexItem::RelativeToCurrentDate);
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = today.addDays(7);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today.addDays(-4));
    }

    {
        // Marking an item which recurs monthly should re-schedule to the following month
        ComplexItem item;
        auto today = QDate::currentDate().startOfDay();
        item.setDueTo(today);
        item.setRecurrencePattern(ComplexItem::RecurMonthly);
        QCOMPARE(item.dueTo(), item.effectiveDueTo());
        QSignalSpy nextDueToChanged(&item, &ComplexItem::nextDueToChanged);
        QSignalSpy effectiveDueToChanged(&item, &ComplexItem::effectiveDueToChanged);
        item.markCurrentOccurrenceAsDone(today);
        QCOMPARE(nextDueToChanged.count(), 1);
        QCOMPARE(effectiveDueToChanged.count(), 1);
        auto expectedNextDate = today.addMonths(1);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today);

        // If the item is overdue, the standard scheduling should still just schedule into the next
        // month:
        item.setDueTo(QDate(2020, 1, 1).startOfDay());
        today = QDate(2020, 2, 15).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 3, 1).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 1).startOfDay());

        // If the item is in the future, re-schedule by one month:
        item.setDueTo(QDate(2020, 1, 4).startOfDay());
        today = QDate(2020, 1, 1).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 2, 4).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 4).startOfDay());

        // Using scheduling relative to today, the next occurrence should be one month in the future
        // from now:
        item.setDueTo(QDate(2020, 1, 1).startOfDay());
        item.setRecurrenceSchedule(ComplexItem::RelativeToCurrentDate);
        today = QDate(2020, 2, 15).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 3, 15).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 1).startOfDay());
    }

    {
        // Marking an item which recurs yearly should re-schedule to the following month
        ComplexItem item;
        auto today = QDate::currentDate().startOfDay();
        item.setDueTo(today);
        item.setRecurrencePattern(ComplexItem::RecurYearly);
        QCOMPARE(item.dueTo(), item.effectiveDueTo());
        QSignalSpy nextDueToChanged(&item, &ComplexItem::nextDueToChanged);
        QSignalSpy effectiveDueToChanged(&item, &ComplexItem::effectiveDueToChanged);
        item.markCurrentOccurrenceAsDone(today);
        QCOMPARE(nextDueToChanged.count(), 1);
        QCOMPARE(effectiveDueToChanged.count(), 1);
        auto expectedNextDate = today.addYears(1);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today);

        // If the item is overdue, the standard scheduling should still just schedule into the next
        // year:
        item.setDueTo(QDate(2020, 1, 1).startOfDay());
        today = QDate(2020, 2, 15).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2021, 1, 1).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 1).startOfDay());

        // If the item is in the future, re-schedule by one year:
        item.setDueTo(QDate(2020, 1, 4).startOfDay());
        today = QDate(2020, 1, 1).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2021, 1, 4).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 4).startOfDay());

        // Using scheduling relative to today, the next occurrence should be one month in the future
        // from now:
        item.setDueTo(QDate(2020, 1, 1).startOfDay());
        item.setRecurrenceSchedule(ComplexItem::RelativeToCurrentDate);
        today = QDate(2020, 2, 15).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2021, 2, 15).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 1).startOfDay());
    }

    {
        // Marking an item which recurs every N days should be scheduled N - something days in the
        // future depending on the current date:
        ComplexItem item;
        auto today = QDate(2020, 1, 1).startOfDay();
        item.setDueTo(today);
        item.setRecurrencePattern(ComplexItem::RecurEveryNDays);
        item.setRecurInterval(14);
        QCOMPARE(item.dueTo(), item.effectiveDueTo());
        QSignalSpy nextDueToChanged(&item, &ComplexItem::nextDueToChanged);
        QSignalSpy effectiveDueToChanged(&item, &ComplexItem::effectiveDueToChanged);
        item.markCurrentOccurrenceAsDone(today);
        QCOMPARE(nextDueToChanged.count(), 1);
        QCOMPARE(effectiveDueToChanged.count(), 1);
        auto expectedNextDate = today.addDays(14);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today);

        // If the item is overdue, the standard scheduling should still just schedule some few days
        // into the future:
        item.setDueTo(QDate(2020, 1, 1).startOfDay());
        today = QDate(2020, 2, 3).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 2, 12).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 1).startOfDay());

        // Re-scheduling an item which is still in the future should add N days:
        item.setDueTo(QDate(2020, 1, 4).startOfDay());
        today = QDate(2020, 1, 1).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 1, 18).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 4).startOfDay());

        // Using scheduling relative to today, the next occurrence should be 14 days from today:
        item.setDueTo(QDate(2020, 1, 2).startOfDay());
        item.setRecurrenceSchedule(ComplexItem::RelativeToCurrentDate);
        today = QDate(2020, 2, 3).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 2, 17).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 2).startOfDay());

        // If the item has an invalid interval set, marking it as done should remove
        // the due date:
        item.setDueTo(QDateTime::currentDateTime());
        item.setRecurInterval(0);
        item.markCurrentOccurrenceAsDone();
        QVERIFY(item.dueTo().isNull());
        QVERIFY(item.effectiveDueTo().isNull());
        QVERIFY(item.nextDueTo().isNull());
    }

    {
        // Marking an item which recurs every N weeks should be scheduled N weeks - something days
        // in the future depending on the current date:
        ComplexItem item;
        auto today = QDate(2020, 1, 1).startOfDay();
        item.setDueTo(today);
        item.setRecurrencePattern(ComplexItem::RecurEveryNWeeks);
        item.setRecurInterval(2);
        QCOMPARE(item.dueTo(), item.effectiveDueTo());
        QSignalSpy nextDueToChanged(&item, &ComplexItem::nextDueToChanged);
        QSignalSpy effectiveDueToChanged(&item, &ComplexItem::effectiveDueToChanged);
        item.markCurrentOccurrenceAsDone(today);
        QCOMPARE(nextDueToChanged.count(), 1);
        QCOMPARE(effectiveDueToChanged.count(), 1);
        auto expectedNextDate = today.addDays(14);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today);

        // If the item is overdue, the standard scheduling should still just schedule some few days
        // into the future:
        item.setDueTo(QDate(2020, 1, 1).startOfDay());
        today = QDate(2020, 2, 3).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 2, 12).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 1).startOfDay());

        // Re-scheduling an item which is still in the future should add N days:
        item.setDueTo(QDate(2020, 1, 4).startOfDay());
        today = QDate(2020, 1, 1).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 1, 18).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 4).startOfDay());

        // Using scheduling relative to today, the next occurrence should be 14 days from today:
        item.setDueTo(QDate(2020, 1, 2).startOfDay());
        item.setRecurrenceSchedule(ComplexItem::RelativeToCurrentDate);
        today = QDate(2020, 2, 3).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 2, 17).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 2).startOfDay());

        // If the item has an invalid interval set, marking it as done should remove
        // the due date:
        item.setDueTo(QDateTime::currentDateTime());
        item.setRecurInterval(0);
        item.markCurrentOccurrenceAsDone();
        QVERIFY(item.dueTo().isNull());
        QVERIFY(item.effectiveDueTo().isNull());
        QVERIFY(item.nextDueTo().isNull());
    }

    {
        // Marking an item which recurs every N months should be scheduled N months - something days
        // in the future depending on the current date:
        ComplexItem item;
        auto today = QDate(2020, 1, 1).startOfDay();
        item.setDueTo(today);
        item.setRecurrencePattern(ComplexItem::RecurEveryNMonths);
        item.setRecurInterval(3);
        QCOMPARE(item.dueTo(), item.effectiveDueTo());
        QSignalSpy nextDueToChanged(&item, &ComplexItem::nextDueToChanged);
        QSignalSpy effectiveDueToChanged(&item, &ComplexItem::effectiveDueToChanged);
        item.markCurrentOccurrenceAsDone(today);
        QCOMPARE(nextDueToChanged.count(), 1);
        QCOMPARE(effectiveDueToChanged.count(), 1);
        auto expectedNextDate = today.addMonths(3);
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.effectiveDueTo(), QDate(2020, 4, 1).startOfDay());
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), today);

        // If the item is overdue, the standard scheduling should still just schedule some few days
        // into the future:
        item.setDueTo(QDate(2020, 1, 2).startOfDay());
        today = QDate(2020, 2, 3).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 4, 2).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 2).startOfDay());

        // Re-scheduling an item which is still in the future should add N days:
        item.setDueTo(QDate(2020, 1, 4).startOfDay());
        today = QDate(2020, 1, 1).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 4, 4).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 4).startOfDay());

        // Using scheduling relative to today, the next occurrence should be 14 days from today:
        item.setDueTo(QDate(2020, 1, 2).startOfDay());
        item.setRecurrenceSchedule(ComplexItem::RelativeToCurrentDate);
        today = QDate(2020, 2, 3).startOfDay();
        item.markCurrentOccurrenceAsDone(today);
        expectedNextDate = QDate(2020, 5, 3).startOfDay();
        QCOMPARE(item.effectiveDueTo(), expectedNextDate);
        QCOMPARE(item.nextDueTo(), expectedNextDate);
        QCOMPARE(item.dueTo(), QDate(2020, 1, 2).startOfDay());

        // If the item has an invalid interval set, marking it as done should remove
        // the due date:
        item.setDueTo(QDateTime::currentDateTime());
        item.setRecurInterval(0);
        item.markCurrentOccurrenceAsDone();
        QVERIFY(item.dueTo().isNull());
        QVERIFY(item.effectiveDueTo().isNull());
        QVERIFY(item.nextDueTo().isNull());
    }
}

QTEST_MAIN(ComplexItemTest)
#include "test_complexitem.moc"
