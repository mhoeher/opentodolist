/*
 * Copyright 2020-2021 Martin Hoeher <martin@rpdev.net>
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

#ifndef DATAMODEL_COMPLEXITEM_H_
#define DATAMODEL_COMPLEXITEM_H_

#include <QDateTime>
#include <QLoggingCategory>
#include <QObject>
#include <QFutureWatcher>

#include "item.h"

/**
 * @brief Complex items.
 *
 * The ComplexItem class is used to model more complex items. This class introduces some additional
 * properties that are not required by the simpler Item class but that nevertheless are common to
 * most other item types.
 */
class ComplexItem : public Item
{
    Q_OBJECT

    Q_PROPERTY(QDateTime dueTo READ dueTo WRITE setDueTo NOTIFY dueToChanged)
    Q_PROPERTY(QString notes READ notes WRITE setNotes NOTIFY notesChanged)
    Q_PROPERTY(QStringList attachments READ attachments NOTIFY attachmentsChanged)
    Q_PROPERTY(RecurrencePattern recurrencePattern READ recurrencePattern WRITE setRecurrencePattern
                       NOTIFY recurrencePatternChanged)
    Q_PROPERTY(RecurrenceSchedule recurrenceSchedule READ recurrenceSchedule WRITE
                       setRecurrenceSchedule NOTIFY recurrenceScheduleChanged)
    Q_PROPERTY(QDateTime nextDueTo READ nextDueTo WRITE setNextDueTo NOTIFY nextDueToChanged)
    Q_PROPERTY(
            int recurInterval READ recurInterval WRITE setRecurInterval NOTIFY recurIntervalChanged)
    Q_PROPERTY(QDateTime effectiveDueTo READ effectiveDueTo NOTIFY effectiveDueToChanged)
    Q_PROPERTY(bool isRecurring READ isRecurring NOTIFY isRecurringChanged)
public:
    /**
     * @brief Determines the recurrence pattern of an item with a due date set.
     */
    enum RecurrencePattern {
        NoRecurrence = 0, //!< The item does not recur.
        RecurDaily, //!< The item recurs daily.
        RecurWeekly, //!< The item recurs weekly.
        RecurMonthly, //!< The item recurs monthly.
        RecurYearly, //!< The item recurs monthly.
        RecurEveryNDays, //!< The item recurs every N days.
        RecurEveryNWeeks, //!< The item recurs every N weeks.
        RecurEveryNMonths, //!< The item recurs every N months.
    };

    Q_ENUM(RecurrencePattern)

    /**
     * @brief Determines the offset to when the next occurrence of the item is scheduled.
     */
    enum RecurrenceSchedule {
        RelativeToOriginalDueDate, //!< The item is scheduled relative to the original due date.
        RelativeToCurrentDate //!< The item is scheduled relative to the current date.
    };

    Q_ENUM(RecurrenceSchedule)

    explicit ComplexItem(QObject* parent = nullptr);
    explicit ComplexItem(const QString& filename, QObject* parent = nullptr);
    explicit ComplexItem(const QDir& dir, QObject* parent = nullptr);
    ~ComplexItem() override;

    QUuid parentId() const override;

    QDateTime dueTo() const;
    void setDueTo(const QDateTime& dueTo);

    QString notes();
    void setNotes(const QString& notes);

    QStringList attachments() const;
    Q_INVOKABLE QString attachmentFileName(const QString& filename);

    // Item interface
    bool deleteItem() override;
    Item* copyTo(const QDir& targetDirectory, const QUuid& targetLibraryUuid,
                 const QUuid& targetItemUid = QUuid()) override;

    RecurrencePattern recurrencePattern() const;
    void setRecurrencePattern(const RecurrencePattern& recurrencePattern);

    RecurrenceSchedule recurrenceSchedule() const;
    void setRecurrenceSchedule(const RecurrenceSchedule& recurrenceSchedule);

    QDateTime nextDueTo() const;
    void setNextDueTo(const QDateTime& nextDueTo);

    int recurInterval() const;
    void setRecurInterval(int recurInterval);

    QDateTime effectiveDueTo() const;
    bool isRecurring() const;

    QDateTime earliestChildDueTo() const;
    void setEarliestChildDueTo(const QDateTime& earliestChildDueTo);

signals:

    void dueToChanged();
    void notesChanged();
    void attachmentsChanged();
    void recurrencePatternChanged();
    void recurrenceScheduleChanged();
    void nextDueToChanged();
    void recurIntervalChanged();
    void effectiveDueToChanged();
    void isRecurringChanged();
    void earliestChildDueToChanged();

public slots:

    void attachFile(const QString& filename);
    void detachFile(const QString& filename);
    void markCurrentOccurrenceAsDone(const QDateTime& today = QDateTime());

protected:
private:
    QDateTime m_dueTo;
    QString m_notes;
    QStringList m_attachments;
    QDateTime m_earliestChildDueTo;

    // Recurrence handling:
    RecurrencePattern m_recurrencePattern;
    RecurrenceSchedule m_recurrenceSchedule;
    QDateTime m_nextDueTo;
    int m_recurInterval;

    void setupConnections();
    void setAttachments(const QStringList& attachments);

protected:
    // Item interface
    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;
    virtual void markItemAsDone();
};

typedef QSharedPointer<ComplexItem> ComplexItemPtr;

#endif // DATAMODEL_COMPLEXITEM_H_
