#include "todolist.h"

#include <QQmlEngine>

#include "library.h"
#include "todo.h"
#include "fileutils.h"

/**
 * @brief Constructor.
 */
TodoList::TodoList(QObject *parent) : TodoList(QString(), parent) {}

/**
 * @brief Constructor.
 */
TodoList::TodoList(const QString &filename, QObject *parent) : TopLevelItem(filename, parent) {}

/**
 * @brief Constructor.
 */
TodoList::TodoList(const QDir &dir, QObject *parent) : TopLevelItem(dir, parent) {}

/**
 * @brief Destructor.
 */
TodoList::~TodoList() {}
