#ifndef TODOLIST_H
#define TODOLIST_H

#include "todoslist.h"
#include "toplevelitem.h"

#include <QObject>
#include <QQmlListProperty>


class TodoList : public TopLevelItem
{
  Q_OBJECT
  Q_PROPERTY(QQmlListProperty<Todo> todos READ todoList NOTIFY todosChanged)
public:
  
  static const QString ItemType;
  static const QStringList PersistentProperties;
  
  explicit TodoList(const QString &directory = QString(), QObject *parent = 0);
  
  Todo* addTodo(const QString &title);
  TodosList todos();
  QQmlListProperty<Todo> todoList();
  
signals:
  
  /**
     @brief The list of todos changed.
   */
  void todosChanged();
  
public slots:
  
private:
  
  TodosList m_todos;
  bool m_todosLoaded;
  
  void appendTodo(Todo *todo);
  bool containsTodo(const QUuid &uuid);
  void loadTodos();
  
  static int todoListCount(QQmlListProperty<Todo> *property);
  static Todo* todoListAt(QQmlListProperty<Todo> *property, int index);
  
private slots:
  
  void onTodoDeleted(Item *item);
  
};

#endif // TODOLIST_H
