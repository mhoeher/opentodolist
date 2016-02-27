#ifndef MIGRATOR_1_X_TO_2_X_H
#define MIGRATOR_1_X_TO_2_X_H

#include <QObject>

// Forward declaration:
class Library;
class TodoList;
class Todo;
class Task;

/**
   @brief Imports files from OpenTodoList versions prior to 2.x.
 */
class Migrator_1_x_to_2_x : public QObject
{
  Q_OBJECT
public:
  explicit Migrator_1_x_to_2_x(Library *targetLibrary, QObject *parent = 0);
  
  void migrate();
  
signals:
  
public slots:
  
private:
  
  Library *m_targetLibrary;
  
  QString localStorageLocation(const QString &type);
  void importTodoList(const QString &dir) const;
  void importTodos(TodoList *todoList, const QString &path) const;
  void importTodo(TodoList *todoList, const QString &fileName) const;
  void importTasks(Todo *todo, const QString &path) const;
  void importTask(Todo *todo, const QString &fileName) const;
};

#endif // MIGRATOR_1_X_TO_2_X_H
