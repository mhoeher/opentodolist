#include "note.h"

/**
   @brief The item type name used by the Note class.
 */
const QString Note::ItemType = "Note";

/**
   @brief Constructor.
 */
Note::Note(const QString &directory, QObject *parent) :
  TopLevelItem(directory, ItemType, QStringList(), parent)
{
}
