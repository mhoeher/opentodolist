#include "task.h"

/**
   @brief The item type used by the Task class.
 */
const QString Task::ItemType = "Task";

/**
  @brief The list of persistent attributes of the Task class.
  */
const QStringList Task::PersistentProperties = {"done"};

/**
   @brief Constructor.
 */
Task::Task(const QString &directory, QObject *parent) : 
  Item(false, directory, ItemType, PersistentProperties, parent),
  m_done(false)
{
  initializeItem();
}

/**
   @brief Set the done state of the task.
 */
void Task::setDone(bool done)
{
  if (m_done != done) {
    m_done = done;
    emit doneChanged();
    saveItem();
  }
}
