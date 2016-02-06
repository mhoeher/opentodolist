#ifndef TODO_H
#define TODO_H

#include "complexitem.h"
#include "tasklist.h"

#include <QObject>
#include <QPointer>


// Forward declaration:
class Task;
class TodoList;



/**
   @brief A single todo inside a todo list.
   
   A todo represents an item in a TodoList. Besides a title, due date and description, each todo
   has a done state. Furthermore, a todo can have arbitrary many sub Task objects.
 */
class Todo : public ComplexItem
{
  Q_OBJECT
  Q_PROPERTY(bool done READ done WRITE setDone NOTIFY doneChanged)
  
  friend class TodoList;
  
public:
  
  static const QString ItemType;
  static const QStringList PersistentProperties;
  
  explicit Todo(const QString &directory = QString(), QObject *parent = 0);
  
  /**
     @brief Return whether the todo is done or not.
   */
  bool done() const { return m_done; }
  void setDone(bool done);
  
  TodoList* todoList() const;
  
  Q_INVOKABLE Task* addTask(const QString &title);
  TaskList tasks();
  
signals:

  /**
     @brief The value of the done property has changed.
   */  
  void doneChanged();
  
public slots:
  
private:
  
  bool m_done;
  TaskList m_tasks;
  bool m_tasksLoaded;
  QPointer<TodoList> m_todoList;
  
  void appendTask(Task *task);
  void loadTasks();
  bool hasTask(const QUuid &uuid);
  void setTodoList(TodoList *todoList);
  
private slots:
  
  void onTaskDeleted(Item *item);
  
};



#endif // TODO_H
