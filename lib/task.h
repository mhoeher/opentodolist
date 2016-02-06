#ifndef TASK_H
#define TASK_H

#include "item.h"

#include <QList>
#include <QObject>
#include <QPointer>


// Forward declaration:
class Todo;


/**
   @brief Represents a task inside a todo.
   
   The Task class represents a single task inside a todo. A task simply is a line of text
   and a flag indicating whether or not the task is done. Hence, tasks model a simple check list
   within a todo.
 */
class Task : public Item
{
  Q_OBJECT
  Q_PROPERTY(bool done READ done WRITE setDone NOTIFY doneChanged)
  Q_PROPERTY(Todo* todo READ todo NOTIFY todoChanged)
  
  friend class Todo;
  
public:
  
  static const QString ItemType;
  static const QStringList PersistentProperties;
  
  explicit Task(const QString &directory, QObject *parent = 0);

  /**
     @brief Indicate whether the task is done or not.
   */
  bool done() const { return m_done; }
  void setDone(bool done);
  
  Todo* todo() const;
  
signals:
  
  /**
     @brief The done state of the task changed.
   */
  void doneChanged();
  
  /**
     @brief The todo property has changed.
   */
  void todoChanged();
  
public slots:
  
private:
  
  bool m_done;
  QPointer<Todo> m_todo;
  
  void setTodo(Todo *todo);
  
};

#endif // TASK_H
