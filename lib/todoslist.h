#ifndef TODOSLIST_H
#define TODOSLIST_H

#include <QList>

// Forward declaration:
class Todo;

/**
   @brief A list of Todo objects.
 */
typedef QList<Todo*> TodosList;

#endif // TODOSLIST_H
