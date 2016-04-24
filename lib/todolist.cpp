#include "todolist.h"

#include "todo.h"
#include "fileutils.h"

#include <QDir>
#include <QMetaMethod>
#include <QMetaObject>
#include <QSet>


const QString TodoList::ItemType = "TodoList";
const QStringList TodoList::PersistentProperties = {};

TodoList::TodoList(const QString &directory, QObject *parent) :
  TopLevelItem(false, directory, ItemType, PersistentProperties, parent),
  m_todos(),
  m_todosLoaded(false)
{
  initializeItem();
}

Todo *TodoList::addTodo(const QString &title)
{
  QString todosDir = directory() + "/todos/";
  QString tpl = titleToDirectoryName(title);
  int i = 0;
  QString todoDir = todosDir + tpl;
  while (QDir(todoDir).exists()) {
    todoDir = todosDir + tpl + " - " + QString::number(i++);
  }
  Todo *todo = new Todo(todoDir, this);
  Q_CHECK_PTR(todo);
  todo->setTitle(title);
  appendTodo(todo);
  todo->saveItem(SaveItemImmediately);
  return todo;
}

TodosList TodoList::todos()
{
  loadTodos();
  return m_todos;
}

QQmlListProperty<Todo> TodoList::todoList()
{
  return QQmlListProperty<Todo>(this, nullptr, todoListCount, todoListAt);
}

void TodoList::handleFileChanged(const QString &filename)
{
  for (auto todo : m_todos) {
    if (FileUtils::isSubDirOrFile(todo->directory(), filename)) {
      todo->handleFileChanged(filename);
      return;
    }
  }
  deleteDanglingTodos();
  scanTodos();
  TopLevelItem::handleFileChanged(filename);
}

bool TodoList::deleteItemData()
{
    bool result = true;
    for (int i = m_todos.length() - 1; i >= 0; --i)
    {
        result = m_todos[i]->deleteItem() && result;
    }
    QDir dir(directory());
    if (dir.exists() && dir.exists("todos"))
    {
        result = dir.rmdir("todos") && result;
    }
    result = TopLevelItem::deleteItemData() && result;
    return result;
}

void TodoList::appendTodo(Todo *todo)
{
  Q_CHECK_PTR(todo);
  todo->setTodoList(this);
  connect(todo, &Item::itemDeleted, this, &TodoList::onTodoDeleted);
  connect(todo, &QObject::destroyed, this, &TodoList::onItemDeleted);
  {
      // Listen to property changed (required for models):
      QMetaMethod targetSlot;
      auto myMeta = metaObject();
      for (int i = 0; i < myMeta->methodCount(); ++i)
      {
          auto method = myMeta->method(i);
          if (method.name() == "handleTodoChanged")
          {
              targetSlot = method;
              break;
          }
      }
      if (targetSlot.isValid())
      {
          auto meta = todo->metaObject();
          for (int i = 0; i < meta->propertyCount(); ++i)
          {
              QMetaProperty property = meta->property(i);
              if (property.notifySignal().isValid())
              {
                  connect(todo, property.notifySignal(),
                          this, targetSlot);
              }
          }
      }
  }
  m_todos.append(todo);
  emit todosChanged();
  emit todoAdded();
}

bool TodoList::containsTodo(const QUuid &uuid)
{
  for (auto todo : m_todos) {
    if (todo->uid() == uuid) {
      return true;
    }
  }
  return false;
}

void TodoList::loadTodos()
{
  if (!m_todosLoaded) {
    m_todosLoaded = true;
    scanTodos();
  }
}

void TodoList::deleteDanglingTodos()
{
  for (auto todo : m_todos) {
    if (todo->isDangling()) {
      todo->deleteItem();
    }
  }
}

void TodoList::scanTodos()
{
  QSet<QString> todoDirs;
  for (auto todo : m_todos) {
    todoDirs.insert(todo->directory());
  }
  QDir todosDir(directory());
  if (todosDir.cd("todos")) {
    for (auto entry : todosDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      QString todoDir = todosDir.absoluteFilePath(entry);
      if (!todoDirs.contains(todoDir)) {
        if (isItemDirectory<Todo>(todoDir)) {
          Todo *todo = new Todo(todoDir, this);
          Q_CHECK_PTR(todo);
          if (containsTodo(todo->uid()))
          {
              delete todo;
          }
          else
          {
              appendTodo(todo);
          }
        }
      }
    }
  }
}

int TodoList::todoListCount(QQmlListProperty<Todo> *property)
{
  TodoList *_this = dynamic_cast<TodoList*>(property->object);
  Q_CHECK_PTR(_this);
  return _this->todos().size();
}

Todo *TodoList::todoListAt(QQmlListProperty<Todo> *property, int index)
{
  TodoList *_this = dynamic_cast<TodoList*>(property->object);
  Q_CHECK_PTR(_this);
  return _this->todos().at(index);
}

void TodoList::onTodoDeleted(Item *item)
{
  Q_CHECK_PTR(item);
  for (int i = 0; i < m_todos.size(); ++i) {
    auto todo = m_todos.at(i);
    if (todo == item) {
      todo->deleteLater();
      m_todos.removeAt(i);
      emit todosChanged();
      emit todoDeleted(i);
      break;
    }
  }
}

void TodoList::onItemDeleted(QObject* item)
{
    Q_CHECK_PTR(item);
    for (int i = 0; i < m_todos.length(); ++i)
    {
        if (m_todos.at(i) == item)
        {
            m_todos.removeAt(i);
            emit todoDeleted(i);
            emit todosChanged();
        }
    }
}

void TodoList::handleTodoChanged()
{
    Todo *todo = dynamic_cast<Todo*>(sender());
    if (todo != nullptr)
    {
        emit todoChanged(todo);
    }
}
