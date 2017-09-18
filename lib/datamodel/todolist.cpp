#include "todolist.h"

#include <QQmlEngine>

#include "library.h"
#include "todo.h"
#include "fileutils.h"


/**
 * @brief Constructor.
 */
TodoList::TodoList(QObject* parent) : TodoList(QString(), parent)
{
}

/**
 * @brief Constructor.
 */
TodoList::TodoList(const QString& filename, QObject* parent) : TopLevelItem(filename, parent),
    m_library(nullptr)
{
}

/**
 * @brief Constructor.
 */
TodoList::TodoList(const QDir& dir, QObject* parent) : TopLevelItem(dir, parent),
    m_library(nullptr)
{
}

/**
 * @brief Destructor.
 */
TodoList::~TodoList()
{
}

/**
 * @brief Create a new todo.
 *
 * This creates a new todo in the library and returns a pointer to it.
 * Note that this method is intended to be called by QML. It is safe to
 * use the returned object in JavaScript/QML. When invoked from C++, do not
 * attempt to store the returned object for access over a longer time,
 * as the library might delete it at any time if need be and hence the pointer
 * might get dangling.
 */
Todo *TodoList::addTodo()
{
    if (m_library != nullptr) {
        TodoPtr todo;
        if (m_library->isValid()) {
            QDir dir(m_library->newItemLocation());
            dir.mkpath(".");
            todo = TodoPtr(new Todo(dir));
        } else {
            todo = TodoPtr(new Todo());
        }
        todo->m_library = m_library;
        todo->setTodoListUid(uid());
        todo->setWeight(m_library->todos()->nextItemWeight());
        m_library->todos()->addItem(todo);
        QQmlEngine::setObjectOwnership(todo.data(), QQmlEngine::CppOwnership);
        todo->save();
        return todo.data();
    }
    return nullptr;
}

Library *TodoList::library() const
{
    return m_library;
}
