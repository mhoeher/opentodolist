/*
 * Copyright 2020-2023 Martin Hoeher <martin@rpdev.net>
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

#include "complexitem.h"

#include <QGuiApplication>
#include <QMimeDatabase>
#include <QTextDocument>
#include <QtConcurrent>

static Q_LOGGING_CATEGORY(log, "OpenTodoList.ComplexItem", QtWarningMsg)

        /**
         * @brief Constructor.
         */
        ComplexItem::ComplexItem(QObject* parent)
    : ComplexItem(QString(), parent)
{
}

/**
 * @brief Constructor.
 */
ComplexItem::ComplexItem(const QString& filename, QObject* parent)
    : Item(filename, parent),
      m_dueTo(),
      m_notes(),
      m_attachments(),
      m_earliestChildDueTo(),
      m_recurrencePattern(NoRecurrence),
      m_recurrenceSchedule(RelativeToOriginalDueDate),
      m_nextDueTo(),
      m_recurUntil(),
      m_recurInterval(0),
      m_newRecurrenceCreated(false)
{
    setupConnections();
}

/**
 * @brief Constructor.
 */
ComplexItem::ComplexItem(const QDir& dir, QObject* parent)
    : Item(dir, parent),
      m_dueTo(),
      m_notes(),
      m_attachments(),
      m_earliestChildDueTo(),
      m_recurrencePattern(NoRecurrence),
      m_recurrenceSchedule(RelativeToOriginalDueDate),
      m_nextDueTo(),
      m_recurUntil(),
      m_recurInterval(0),
      m_newRecurrenceCreated(false)
{
    setupConnections();
}

/**
 * @brief Destructor.
 */
ComplexItem::~ComplexItem() {}

QUuid ComplexItem::parentId() const
{
    return QUuid();
}

/**
 * @brief The due date and time of the item.
 */
QDateTime ComplexItem::dueTo() const
{
    return m_dueTo;
}

/**
 * @brief Set the due date and time of the item.
 */
void ComplexItem::setDueTo(const QDateTime& dueTo)
{
    if (m_dueTo != dueTo || effectiveDueTo() != dueTo) {
        if (m_nextDueTo.isValid()) {
            // Reset next due to as soon as user explicitly selects a new date:
            setNextDueTo(QDateTime());
        }
        m_dueTo = dueTo;
        // Note: Save only on a second basis.
        QTime t = dueTo.time();
        t.setHMS(t.hour(), t.minute(), t.second(), 0);
        m_dueTo.setTime(t);

        // If the item already has a recurUntil set, check if the new due date occurs after it.
        // If this is the case, reset the recurUntil:
        if (m_recurUntil.isValid() && m_dueTo >= m_recurUntil) {
            setRecurUntil(QDateTime());
        }

        emit dueToChanged();
    }
}

/**
 * @brief Return the notes (as HTML) attached to the item.
 */
QString ComplexItem::notes()
{
    return m_notes;
}

/**
 * @brief Set the notes (as HTML) of the item.
 */
void ComplexItem::setNotes(const QString& notes)
{
    auto copy = notes;
    // Detect old-style HTML notes and convert to plain text:
    if (copy.startsWith("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" "
                        "\"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                        "<html><head><meta name=\"qrichtext\" content=\"1\""
                        " />")) {
        auto guiApp = qobject_cast<QGuiApplication*>(QCoreApplication::instance());
        if (guiApp) {
            QTextDocument doc;
            doc.setHtml(copy);
            copy = doc.toPlainText();
        }
    }
    if (m_notes != copy) {
        m_notes = copy;
        emit notesChanged();
    }
}

QStringList ComplexItem::attachments() const
{
    return m_attachments;
}

QString ComplexItem::attachmentFileName(const QString& filename)
{
    QString result;
    if (isValid()) {
        QDir dir(directory());
        result = dir.absoluteFilePath(filename);
    }
    return result;
}

/**
 * @brief Attach a file to the item.
 *
 * This attaches the file (itentified by its @p filename) to the item.
 * Attaching means that the file is copied next to the item's main file.
 */
void ComplexItem::attachFile(const QString& filename)
{
    if (isValid()) {
        QFileInfo fi(filename);
        if (fi.exists() && fi.isFile() && fi.isReadable()) {
            QDir dir(directory());
            if (!dir.exists()) {
                if (!dir.mkpath(".")) {
                    qCWarning(log)
                            << "Failed to create item directory" << dir << "for attaching file";
                    return;
                }
            }
            QString targetFileName = fi.fileName();
            int i = 0;
            auto completeSuffix = fi.completeSuffix();
            if (completeSuffix.isEmpty()) {
                // Could happen, e.g. on Android, due to content:// URLs. Try to guess from mimetype
                QMimeDatabase mdb;
                auto mimetype = mdb.mimeTypeForFile(filename);
                if (mimetype.isValid()) {
                    completeSuffix = mimetype.preferredSuffix();
                    targetFileName = fi.baseName() + "." + completeSuffix;
                }
            }
            while (dir.exists(targetFileName)) {
                ++i;
                targetFileName = fi.baseName() + "-" + QString::number(i) + "." + completeSuffix;
            }
            QFile::copy(filename, dir.absoluteFilePath(targetFileName)); // NOLINT
            m_attachments.append(targetFileName);
            std::stable_sort(m_attachments.begin(), m_attachments.end());
            emit attachmentsChanged();
        } else {
            qCWarning(log) << "Cannot attach file" << filename << "to" << uid()
                           << ": File does not exist or cannot be read.";
        }
    }
}

void ComplexItem::attachFile(const QUrl& url)
{
    if (url.scheme() == "content") {
        // On Android, pass-thru the content:// URL as is, see
        // https://www.volkerkrause.eu/2019/02/16/qt-open-files-on-android.html
        attachFile(url.toString());
    } else {
        attachFile(url.toLocalFile());
    }
}

/**
 * @brief Remove an attachment.
 *
 * Remove the attachment identified by the attachment @p filename.
 */
void ComplexItem::detachFile(const QString& filename)
{
    if (isValid()) {
        auto absoluteFilename = attachmentFileName(filename);
        QFileInfo fi(absoluteFilename);
        if (fi.exists() && fi.isWritable()) {
            auto directory = this->directory();
            QDir dir(directory);
            if (!dir.remove(filename)) {}
            m_attachments.removeAll(filename);
            emit attachmentsChanged();
        }
    }
}

/**
 * @brief Mark the current occurrence of the item as done.
 *
 * This marks this item as done. Depending on whether the item has a recurrence pattern set or now
 * and what type of item this is, the behavior is a bit different.
 *
 * - If no recurrence pattern is set, the due date is removed or other, item specific changes are
 *   done to actually mark the item as done.
 * - If the item has a recurrence pattern set, its nextDueTo date is set.
 *
 * Scheduling of the next occurrence is done relative to @p today. If `today` is null, the
 * current date will be used.
 */
void ComplexItem::markCurrentOccurrenceAsDone(const QDateTime& today)
{
    auto _today = today;
    if (_today.isNull() || !_today.isValid()) {
        auto currentDate = QDate::currentDate();
        _today = currentDate.startOfDay();
    }

    if (isRecurring()) {
        // Mark the current occurrence of this item as done
        auto nextDueTo_ = nextEffectiveDueTo(today);
        if (nextDueTo_.isValid()) {
            setNewRecurrenceCreated(true); // Mark the item so we know it was scheduled again
            setNextDueTo(nextDueTo_);
        } else {
            markItemAsDone();
        }
    } else {
        // Simply mark the item as done:
        markItemAsDone();
    }
}

/**
 * @brief Indicates that a new occurrence of the item has been created.
 *
 * This property is true after the item was been marked as done and - if the item has a recurrence
 * pattern set - a new occurrence was scheduled for the future.
 */
bool ComplexItem::newRecurrenceCreated() const
{
    return m_newRecurrenceCreated;
}

void ComplexItem::setNewRecurrenceCreated(bool newNewRecurrenceCreated)
{
    if (m_newRecurrenceCreated == newNewRecurrenceCreated)
        return;
    m_newRecurrenceCreated = newNewRecurrenceCreated;
    emit newRecurrenceCreatedChanged();
}

/**
 * @brief Stop recurring after the specified date.
 *
 * If this is set, recurrence stops after the given date.
 */
const QDateTime& ComplexItem::recurUntil() const
{
    return m_recurUntil;
}

/**
 * @brief Sets the date up to which the item recurs.
 */
void ComplexItem::setRecurUntil(const QDateTime& newRecurUntil)
{
    if (m_recurUntil == newRecurUntil)
        return;
    m_recurUntil = newRecurUntil;
    emit recurUntilChanged();
}

/**
 * @brief Check if the item can be marked as done.
 *
 * This method returns a boolean flag indicating if the item can be marked as done. The
 * ComplexItem class returns true in case the item has an effectiveDueTo date set.
 *
 * Subclasses should extend this method to add class specific logic on top.
 */
bool ComplexItem::canBeMarkedAsDone() const
{
    return effectiveDueTo().isValid();
}

/**
 * @brief The earliest due to date of the direct children of the item.
 *
 * This property holds the earliest due to date among the direct child items of that item.
 */
QDateTime ComplexItem::earliestChildDueTo() const
{
    return m_earliestChildDueTo;
}

/**
 * @brief Set the earliest due to date among the direct children of the item.
 */
void ComplexItem::setEarliestChildDueTo(const QDateTime& earliestChildDueTo)
{
    if (m_earliestChildDueTo != earliestChildDueTo) {
        m_earliestChildDueTo = earliestChildDueTo;
        emit earliestChildDueToChanged();
        emit effectiveDueToChanged();
    }
}

/**
 * @brief The recurrence interval.
 *
 * This is the interval between recurrences of the item.
 *
 * @sa RecurrencePattern::RecurEveryNDays
 * @sa RecurrencePattern::RecurEveryNWeeks
 * @sa RecurrencePattern::RecurEveryNMonths
 */
int ComplexItem::recurInterval() const
{
    return m_recurInterval;
}

/**
 * @brief Set the recurrence pattern.
 */
void ComplexItem::setRecurInterval(int recurInterval)
{
    if (m_recurInterval != recurInterval) {
        m_recurInterval = recurInterval;
        emit recurIntervalChanged();
    }
}

/**
 * @brief Returns the effective next date on which an item is due to.
 *
 * This is a calculated property, which indicates when the item is next due. If an item has
 * no recurrence pattern set, this is equal to the dueTo property. If an item has a recurrence
 * pattern set and a past instance of the item has already been marked as done, this equals the
 * nextDueTo property.
 *
 * In addition, if an item has no own due date but children which might in turn have a due date, the
 * earliestChildDueTo() is returned.
 */
QDateTime ComplexItem::effectiveDueTo() const
{
    if (m_nextDueTo.isValid() && m_dueTo.isValid() && m_nextDueTo > m_dueTo) {
        return m_nextDueTo;
    } else if (m_dueTo.isValid()) {
        return m_dueTo;
    } else {
        return m_earliestChildDueTo;
    }
}

/**
 * @brief Returns the next effective due date of the item.
 *
 * This returns the next due to date of the item if it would be marked as done in its current
 * state. The next date will be calculated relative to @p today (which is the current date by
 * default).
 *
 * This method returns an invalid date if the item has no recurrence set.
 *
 * @note This method returns the recur until date (if set).
 */
QDateTime ComplexItem::nextEffectiveDueTo(const QDateTime& today) const
{
    auto _today = today;
    if (_today.isNull() || !_today.isValid()) {
        auto currentDate = QDate::currentDate();
        _today = currentDate.startOfDay();
    }

    if (isRecurring()) {
        switch (m_recurrencePattern) {

        case NoRecurrence:
            // Should not happen - is implicitly handled in `isRecurring()`.
            break;

        case RecurDaily: {
            auto nextDueToValue = effectiveDueTo().addDays(1);
            if (nextDueToValue <= _today) {
                nextDueToValue = _today.addDays(1);
            }
            return nextDueToValue;
        }

        case RecurWeekly: {
            switch (m_recurrenceSchedule) {
            case RelativeToOriginalDueDate: {
                auto nextDueToValue = m_dueTo;
                while (nextDueToValue <= _today || nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addDays(7);
                }
                return nextDueToValue;
            }
            case RelativeToCurrentDate: {
                auto nextDueToValue = _today.addDays(7);
                while (nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addDays(7);
                }
                return nextDueToValue;
            }
            }
            break;
        }

        case RecurMonthly: {
            switch (m_recurrenceSchedule) {
            case RelativeToOriginalDueDate: {
                auto nextDueToValue = m_dueTo;
                while (nextDueToValue <= _today || nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addMonths(1);
                }
                return nextDueToValue;
            }
            case RelativeToCurrentDate: {
                auto nextDueToValue = _today.addMonths(1);
                while (nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addMonths(1);
                }
                return nextDueToValue;
            }
            }
            break;
        }

        case RecurYearly: {
            switch (m_recurrenceSchedule) {
            case RelativeToOriginalDueDate: {
                auto nextDueToValue = m_dueTo;
                while (nextDueToValue <= _today || nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addYears(1);
                }
                return nextDueToValue;
            }
            case RelativeToCurrentDate: {
                auto nextDueToValue = _today.addYears(1);
                while (nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addYears(1);
                }
                return nextDueToValue;
            }
            }
            break;
        }

        case RecurEveryNDays: {
            switch (m_recurrenceSchedule) {
            case RelativeToOriginalDueDate: {
                auto nextDueToValue = m_dueTo;
                while (nextDueToValue <= _today || nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addDays(m_recurInterval);
                }
                return nextDueToValue;
            }
            case RelativeToCurrentDate: {
                auto nextDueToValue = _today.addDays(m_recurInterval);
                while (nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addDays(m_recurInterval);
                }
                return nextDueToValue;
            }
            }
            break;
        }

        case RecurEveryNWeeks: {
            switch (m_recurrenceSchedule) {
            case RelativeToOriginalDueDate: {
                auto nextDueToValue = m_dueTo;
                while (nextDueToValue <= _today || nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addDays(m_recurInterval * 7);
                }
                return nextDueToValue;
            }
            case RelativeToCurrentDate: {
                auto nextDueToValue = _today.addDays(m_recurInterval * 7);
                while (nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addDays(m_recurInterval * 7);
                }
                return nextDueToValue;
            }
            }
            break;
        }

        case RecurEveryNMonths: {
            switch (m_recurrenceSchedule) {
            case RelativeToOriginalDueDate: {
                auto nextDueToValue = m_dueTo;
                auto eDT = effectiveDueTo();
                while (nextDueToValue <= _today || nextDueToValue <= eDT) {
                    nextDueToValue = nextDueToValue.addMonths(m_recurInterval);
                }
                return nextDueToValue;
            }
            case RelativeToCurrentDate: {
                auto nextDueToValue = _today.addMonths(m_recurInterval);
                while (nextDueToValue <= effectiveDueTo()) {
                    nextDueToValue = nextDueToValue.addMonths(m_recurInterval);
                }
                return nextDueToValue;
            }
            }
            break;
        }
        }
    }

    return QDateTime();
}

/**
 * @brief Indicates if the item recurs.
 *
 * An item recurs in the following cases:
 *
 * - It has a valid due to date set.
 * - It has a recurrence pattern other than RecurrencePattern::NoRecurrence set.
 * - If the recurrence pattern RecurrencePattern::RecurEveryNDays,
 *   RecurrencePattern::RecurEveryNWeeks or RecurrencePattern::RecurEveryNMonths is set, the
 *   recurInterval must be greater than 1.
 */
bool ComplexItem::isRecurring() const
{
    return m_dueTo.isValid() && m_recurrencePattern != NoRecurrence
            && ((m_recurrencePattern != RecurEveryNDays && m_recurrencePattern != RecurEveryNWeeks
                 && m_recurrencePattern != RecurEveryNMonths)
                || m_recurInterval > 0);
}

/**
 * @brief Indicates if the item is a future instance.
 *
 * This property is true when the next effecture due to date of the item is scheduled farther into
 * the future as its recurrence interval. For example:
 *
 * Consider an item which recurs weekly each Wednesday. Assume that we have Monday, with the next
 * due to date on the following Wednesday.
 *
 * - In this state, isFutureInstance() would be false, because the currently scheduled instance
 *   is the "current" one.
 * - Now, when we mark the current occurrence as done, it will be rescheduled to Wednesday the
 *   following week. In this state, the property will in fact be true, because the item is
 *   farther into the future (a week and two days) then the recurrence interval (which is one
 *   week).
 *
 *  @note If the item has no recurrence set, this property is false.
 */
bool ComplexItem::isFutureInstance() const
{
    if (isRecurring()) {
        auto today = QDate::currentDate().startOfDay();
        auto effectiveDueTo_ = this->effectiveDueTo();
        auto nextEffectiveDueTo_ = this->nextEffectiveDueTo();
        auto intervalInDays = effectiveDueTo_.daysTo(nextEffectiveDueTo_);
        auto daysToDueDate = today.daysTo(effectiveDueTo_);

        return daysToDueDate > 0 && intervalInDays <= daysToDueDate;
    } else {
        return false;
    }
}

/**
 * @brief The next due date of the item.
 *
 * This holds the next due date of the item. This property is set when an occurrence of the item is
 * marked as done and the item has a recurrence pattern set.
 */
QDateTime ComplexItem::nextDueTo() const
{
    return m_nextDueTo;
}

/**
 * @brief Set the next due date of the item.
 */
void ComplexItem::setNextDueTo(const QDateTime& nextDueTo)
{
    if (m_nextDueTo != nextDueTo) {
        // Check if we have a valid end date set. If so, stop here and instead mark the
        // item as done:
        if (m_recurUntil.isValid() && m_recurUntil < nextDueTo) {
            markItemAsDone();
            return;
        }

        m_nextDueTo = nextDueTo;
        emit nextDueToChanged();
    }
}

/**
 * @brief Determines how the next due date is selected.
 *
 * This property determines how the next due to date of an item is selected when an instance is
 * marked as done.
 */
ComplexItem::RecurrenceSchedule ComplexItem::recurrenceSchedule() const
{
    return m_recurrenceSchedule;
}

/**
 * @brief Set the recurrence scheduling mode of the item.
 */
void ComplexItem::setRecurrenceSchedule(const RecurrenceSchedule& recurrenceSchedule)
{
    if (m_recurrenceSchedule != recurrenceSchedule) {
        m_recurrenceSchedule = recurrenceSchedule;
        emit recurrenceScheduleChanged();
    }
}

/**
 * @brief The pattern in which the item recurs.
 */
ComplexItem::RecurrencePattern ComplexItem::recurrencePattern() const
{
    return m_recurrencePattern;
}

/**
 * @brief Set the recurrence pattern of the item.
 */
void ComplexItem::setRecurrencePattern(const RecurrencePattern& recurrencePattern)
{
    if (m_recurrencePattern != recurrencePattern) {
        m_recurrencePattern = recurrencePattern;
        emit recurrencePatternChanged();
    }
}

void ComplexItem::setupConnections()
{
    connect(this, &ComplexItem::dueToChanged, this, &ComplexItem::changed);
    connect(this, &ComplexItem::notesChanged, this, &ComplexItem::changed);
    connect(this, &ComplexItem::attachmentsChanged, this, &ComplexItem::changed);
    connect(this, &ComplexItem::recurrencePatternChanged, this, &ComplexItem::changed);
    connect(this, &ComplexItem::recurrenceScheduleChanged, this, &ComplexItem::changed);
    connect(this, &ComplexItem::recurIntervalChanged, this, &ComplexItem::changed);
    connect(this, &ComplexItem::nextDueToChanged, this, &ComplexItem::changed);
    connect(this, &ComplexItem::recurUntilChanged, this, &ComplexItem::changed);

    connect(this, &ComplexItem::dueToChanged, this, &ComplexItem::effectiveDueToChanged);
    connect(this, &ComplexItem::nextDueToChanged, this, &ComplexItem::effectiveDueToChanged);

    connect(this, &ComplexItem::recurrencePatternChanged, this, &ComplexItem::isRecurringChanged);
    connect(this, &ComplexItem::recurIntervalChanged, this, &ComplexItem::isRecurringChanged);
    connect(this, &ComplexItem::dueToChanged, this, &ComplexItem::isRecurringChanged);
}

void ComplexItem::setAttachments(const QStringList& attachments)
{
    if (m_attachments != attachments) {
        m_attachments = attachments;
        emit attachmentsChanged();
    }
}

QVariantMap ComplexItem::toMap() const
{
    auto result = Item::toMap();
    result["dueTo"] = m_dueTo;
    result["notes"] = m_notes;
    result["attachments"] = m_attachments;
    result["recurrencePattern"] = QVariant::fromValue(m_recurrencePattern);
    result["recurrenceSchedule"] = QVariant::fromValue(m_recurrenceSchedule);
    result["nextDueTo"] = m_nextDueTo;
    result["recurUntil"] = m_recurUntil;
    result["recurInterval"] = m_recurInterval;

    return result;
}

void ComplexItem::fromMap(QVariantMap map)
{
    Item::fromMap(map);
    setDueTo(map.value("dueTo", m_dueTo).toDateTime());
    setNotes(map.value("notes", m_notes).toString());
    setAttachments(map.value("attachments", m_attachments).toStringList());
    setRecurrencePattern(map.value("recurrencePattern", QVariant::fromValue(m_recurrencePattern))
                                 .value<RecurrencePattern>());
    setRecurrenceSchedule(map.value("recurrenceSchedule", QVariant::fromValue(m_recurrenceSchedule))
                                  .value<RecurrenceSchedule>());
    setNextDueTo(map.value("nextDueTo", m_nextDueTo).toDateTime());
    setRecurUntil(map.value("recurUntil", m_recurUntil).toDateTime());
    setRecurInterval(map.value("recurInterval", m_recurInterval).toInt());
}

QVariantMap ComplexItem::getRuntimeData() const
{
    auto result = Item::getRuntimeData();
    result["newRecurrenceCreated"] = m_newRecurrenceCreated;
    return result;
}

void ComplexItem::applyRuntimeData(const QVariantMap& runtimeData)
{
    Item::applyRuntimeData(runtimeData);
    setNewRecurrenceCreated(runtimeData.value("newRecurrenceCreated", false).toBool());
}

/**
 * @brief Mark the item as done.
 *
 * This marks this item as "done". By default, this removed any set due dates from the item.
 * Concrete sub-classes can change the behavior to better fit their usage patterns.
 */
void ComplexItem::markItemAsDone()
{
    setDueTo(QDateTime());
}

bool ComplexItem::deleteItem()
{
    if (isValid()) {
        const auto& attachments_ = m_attachments;
        for (const auto& attachment : attachments_) {
            auto path = attachmentFileName(attachment);
            QFile file(path);
            if (file.exists()) {
                if (!file.remove()) {
                    qCWarning(log)
                            << "Failed to remove attachment" << path << ":" << file.errorString();
                }
            }
        }
    }
    return Item::deleteItem();
}

Item* ComplexItem::copyTo(const QDir& targetDirectory, const QUuid& targetLibraryUuid,
                          const QUuid& targetItemUid)
{
    auto result = Item::copyTo(targetDirectory, targetLibraryUuid, targetItemUid);
    auto complexItem = qobject_cast<ComplexItem*>(result);
    if (complexItem) {
        complexItem->m_attachments.clear();
        const auto& attachmentsValue = m_attachments;
        for (const auto& attachment : attachmentsValue) {
            const auto fn = attachmentFileName(attachment);
            qCWarning(log) << "Attaching" << fn << "to copy of" << uid();
            complexItem->attachFile(attachmentFileName(attachment));
        }
        qCWarning(log) << "Attachments of" << complexItem->uid() << ":"
                       << complexItem->attachments();
    }
    return result;
}
