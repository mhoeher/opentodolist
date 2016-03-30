#ifndef COMPLEXITEM_H
#define COMPLEXITEM_H

#include "item.h"

#include <QDateTime>
#include <QObject>

/**
   @brief Complex items.
   
   The ComplexItem class is used to model more complex items. This class introduces some additional
   properties that are not required by the simpler Item class but that nevertheless are common to
   most other item types.
 */
class ComplexItem : public Item
{
  Q_OBJECT
  Q_PROPERTY(QDateTime dueTo READ dueTo WRITE setDueTo NOTIFY dueToChanged)
  Q_PROPERTY(QString dueToAsISO READ dueToAsISO WRITE setDueToAsISO NOTIFY dueToChanged)
  Q_PROPERTY(QString notes READ notes WRITE setNotes NOTIFY notesChanged)
public:
  
  static const QStringList PersistentProperties;
  static const QString ItemType;
  
  explicit ComplexItem(const QString &directory = QString(),
                       const QString &itemType = ItemType,
                       const QStringList &persistentProperties = QStringList(),
                       QObject *parent = 0);
  
  QDateTime dueTo() const
  {
    return m_dueTo;
  }
  void setDueTo(const QDateTime &dueTo);
  
  /**
     @brief Returns the dueTo date and time as ISO formatted date string.
   */
  QString dueToAsISO() const { return m_dueTo.toString(Qt::ISODate); }
  void setDueToAsISO(const QString &dueTo);
  
  QString notes();
  void setNotes(const QString &notes);
  
signals:
  
  void dueToChanged();
  void notesChanged();
  
public slots:
  
protected:
  
  explicit ComplexItem(bool loadItem, 
                       const QString &directory = QString(),
                       const QString &itemType = ItemType,
                       const QStringList &persistentProperties = QStringList(),
                       QObject *parent = 0);
  
private:
  
  QDateTime m_dueTo;
  QString   m_notes;
  bool      m_notesLoaded;
  
  void setupReload();
  
};

#endif // COMPLEXITEM_H
