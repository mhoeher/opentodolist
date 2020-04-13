#ifndef NOTE_H
#define NOTE_H

#include "toplevelitem.h"

#include <QObject>

/**
 * @brief A class representing a single note.
 *
 * The Note class represents a single note inside a library. A note is like a
 * sticky note, whith a short headline and (optionally) some additional text providing
 * further details.
 */
class Note : public TopLevelItem
{
    Q_OBJECT
public:
    explicit Note(QObject *parent = nullptr);
    explicit Note(const QString &filename, QObject *parent = nullptr);
    explicit Note(const QDir &dir, QObject *parent = nullptr);
    virtual ~Note();
};

typedef QSharedPointer<Note> NotePtr;

#endif // NOTE_H
