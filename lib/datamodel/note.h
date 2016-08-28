#ifndef NOTE_H
#define NOTE_H

#include "toplevelitem.h"

#include <QObject>

/**
   @brief A class representing a single note.
   
   The Note class represents a single note inside a library. A note is like a
   sticky note, whith a short headline and (optionally) some additional text providing
   further details.
 */
class Note : public TopLevelItem
{
  Q_OBJECT
public:
  static const QString ItemType;
  
  explicit Note(const QString &directory = QString(),
                QObject *parent = 0);
  
};

#endif // NOTE_H
