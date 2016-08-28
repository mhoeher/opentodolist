#include "complexitem.h"

#include <QDebug>
#include <QFile>
#include <QTimer>

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
    m_notesModified(false),
    m_notesLoaded(false),
    m_notesTimer(new QTimer(this))
{
    initializeItem();
    setupReload();
    setupNotesTimer();
}

ComplexItem::~ComplexItem()
{
    saveNotes(); // Flush notes if necessary
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
        m_notesModified = true;
        m_notesTimer->start();
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
    m_notesModified(false),
    m_notesLoaded(false),
    m_notesTimer(new QTimer(this))
{
    if (loadItem) {
        initializeItem();
    }
    setupReload();
    setupNotesTimer();
}

bool ComplexItem::deleteItemData()
{
    bool result = false;
    QFile notesFile(directory() + "/" + "notes.html");
    if (notesFile.exists()) 
    {
        if (notesFile.remove()) 
        {
            result = true;
        }
    }
    result = Item::deleteItemData() && result;
    return result;
}

void ComplexItem::setupReload()
{
    connect(this, &Item::reloaded, [this] 
    {
        m_notes = QString();
        m_notesLoaded = false;
        emit notesChanged();
    });
}

void ComplexItem::setupNotesTimer()
{
    Q_CHECK_PTR(m_notesTimer);
    m_notesTimer->setInterval(10000);
    m_notesTimer->setSingleShot(true);
    connect(m_notesTimer, &QTimer::timeout, this, &ComplexItem::saveNotes);
}

void ComplexItem::saveNotes()
{
    if (m_notesModified && isValid() && !readonly()) {
        QFile file(directory() + "/notes.html");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_notes.toUtf8());
            file.close();
            m_notesModified = true;
        }
    }
}
