#include "note.h"


/**
 * @brief Constructor.
 */
Note::Note(QObject* parent) : Note(QString(), parent)
{
}

Note::Note(const QString& filename, QObject* parent) : TopLevelItem(filename, parent)
{
}

Note::~Note()
{
}
