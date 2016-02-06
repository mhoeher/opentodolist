#ifndef TODOLIST_H
#define TODOLIST_H

#include "todoslist.h"
#include "toplevelitem.h"

#include <QObject>

class TodoList : public TopLevelItem
{
  Q_OBJECT
public:
  
  static const QString ItemTye;
  static const QStringList PersistentProperties;
  
  explicit TodoList(const QString &directory = QString(), QObject *parent = 0);
  
  Todo* addTodo(const QString &title);
  TodosList todos();
  
signals:
  
public slots:
  
private:
  
  TodosList m_todos;
  bool m_todosLoaded;
  
  void appendTodo(Todo *todo);
  bool containsTodo(const QUuid &uuid);
  void loadTodos();
  
private slots:
  
  void onTodoDeleted(Item *item);
  
};

#endif // TODOLIST_H
