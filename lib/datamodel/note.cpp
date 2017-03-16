#include "note.h"


/**
 * @brief Constructor.
 */
Note::Note(QObject* parent) : Note(QString(), parent)
{
}

/**
 * @brief Constructor.
 */
Note::Note(const QString& filename, QObject* parent) : TopLevelItem(filename, parent)
{
}

/**
 * @brief Constructor.
 */
Note::Note(const QDir& dir, QObject* parent) : TopLevelItem(dir, parent)
{
}

Note::~Note()
{
}
