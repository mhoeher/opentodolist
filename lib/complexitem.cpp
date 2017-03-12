#include "complexitem.h"

/**
 * @brief Constructor.
 */
ComplexItem::ComplexItem(QObject* parent) : ComplexItem(QString(), parent)
{
}

/**
 * @brief Constructor.
 */
ComplexItem::ComplexItem(const QString& filename, QObject* parent) : Item(filename, parent),
    m_dueTo(),
    m_notes()
{
}

/**
 * @brief Destructor.
 */
ComplexItem::~ComplexItem()
{
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
void ComplexItem::setDueTo(const QDateTime &dueTo)
{
    if (m_dueTo != dueTo) {
        m_dueTo = dueTo;
        // Note: Save only on a second basis.
        QTime t = dueTo.time();
        t.setHMS(t.hour(), t.minute(), t.second(), 0);
        m_dueTo.setTime(t);
        emit dueToChanged();
        save();
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
void ComplexItem::setNotes(const QString &notes)
{
    if (m_notes != notes) {
        m_notes = notes;
        emit notesChanged();
        save();
    }
}

QVariantMap ComplexItem::toMap() const
{
    auto result = Item::toMap();
    result["dueTo"] = m_dueTo;
    result["notes"] = m_notes;
    return result;
}

void ComplexItem::fromMap(QVariantMap map)
{
    Item::fromMap(map);
    setDueTo(map.value("dueTo", m_dueTo).toDateTime());
    setNotes(map.value("notes", m_notes).toString());
}
