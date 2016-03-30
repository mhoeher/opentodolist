#include "complexitem.h"

#include <QDebug>
#include <QFile>

/**
  @brief The list of persistent properties of the ComplexItem class.
  */
const QStringList ComplexItem::PersistentProperties = {"dueToAsISO"};

/**
   @brief The type name of the ComplexType class.
 */
const QString ComplexItem::ItemType = "ComplexItem";

/**
   @brief COnstructor.
 */
ComplexItem::ComplexItem(const QString &directory,
                         const QString &itemType,
                         const QStringList &persistentProperties,
                         QObject *parent) :
  Item(false, directory, itemType, persistentProperties + PersistentProperties, parent),
  m_dueTo(QDateTime()),
  m_notes(QString()),
  m_notesLoaded(false)
{
  initializeItem();
  setupReload();
}

/**
   @brief Set the due date and time of the item.
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
    saveItem();
  }
}

/**
   @brief Sets the dueTo date and time via a ISO formatted date string.
 */
void ComplexItem::setDueToAsISO(const QString &dueTo)
{
  setDueTo( QDateTime::fromString(dueTo, Qt::ISODate));
}

/**
   @brief Return the notes (as HTML) attached to the item.
   
   @note This method lazy loads the notes from disk. On first access, this will cause an access
         disk.
 */
QString ComplexItem::notes()
{
  if (!m_notesLoaded) {
    m_notesLoaded = true;
    if (isValid()) {
      QFile file(directory() + "/notes.html");
      if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
          m_notes = file.readAll();
          file.close();
        } else {
          qWarning() << "Failed to open" << file.fileName() << "of" << this;
        }
      } else {
        qDebug() << "The file notes.html of " << this << "does not exist.";
      }
    }
  }
  return m_notes;
}

/**
   @brief Set the notes (as HTML) of the item.
 */
void ComplexItem::setNotes(const QString &notes)
{
  if (m_notes != notes) {
    m_notes = notes;
    emit notesChanged();
    if (isValid() && !readonly()) {
      QFile file(directory() + "/notes.html");
      if (file.open(QIODevice::WriteOnly)) {
        file.write(m_notes.toUtf8());
        file.close();
      }
    }
  }
}

/**
   @brief Constructor.
   
   This is an overloaded constructor. If @p loadItem is set to true, the initializeItem() method 
   will be called by the constructor. If a sub-class passes false, it shall call initializeItem()
   itself in its constructor.
 */
ComplexItem::ComplexItem(bool loadItem,
                         const QString &directory,
                         const QString &itemType,
                         const QStringList &persistentProperties,
                         QObject *parent) :
  Item(false, directory, itemType, persistentProperties + PersistentProperties, parent),
  m_dueTo(QDateTime()),
  m_notes(QString()),
  m_notesLoaded(false)
{
  if (loadItem) {
    initializeItem();
  }
  setupReload();
}

void ComplexItem::setupReload()
{
  connect(this, &Item::reloaded, [this]{
    m_notes = QString();
    m_notesLoaded = false;
    emit notesChanged();
  });
}
