#include "migrator_1_x_to_2_x.h"

#include "library.h"
#include "task.h"
#include "todo.h"
#include "todolist.h"

#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QFileInfo>
#include <QStandardPaths>


Migrator_1_x_to_2_x::Migrator_1_x_to_2_x(Library *targetLibrary, QObject *parent) : 
  QObject(parent),
  m_targetLibrary(targetLibrary)
{ 
}

void Migrator_1_x_to_2_x::migrate()
{
  qDebug() << "Starting migration.";
  QString path = localStorageLocation("LocalXmlDirectory");
  QDir dir(path);
  for (QString entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      importTodoList(dir.absoluteFilePath(entry));
  }
  qDebug() << "Migration finished.";
}


QString Migrator_1_x_to_2_x::localStorageLocation(const QString &type)
{
    QStringList locations = QStandardPaths::standardLocations( QStandardPaths::DataLocation );

#ifdef Q_OS_ANDROID
    QString androidExtStorage(qgetenv("EXTERNAL_STORAGE"));
    QDir dir(androidExtStorage + "/data/net.rpdev.opentodolist/");
    locations.insert(0, dir.absolutePath());
#endif

    QString overridePath( qgetenv( "OPENTODOLIST_LOCAL_STORAGE_LOCATION" ) );
    if ( !overridePath.isEmpty() ) {
      locations.insert( 0, overridePath );
    }

    foreach ( QString location, locations ) {
        QDir dir( location + "/" + type );
        if ( !dir.exists() ) {
            dir.mkpath( dir.absolutePath() );
        }
        if ( dir.exists() ) {
            return dir.path();
        }
    }
    return QString();
}

void Migrator_1_x_to_2_x::importTodoList(const QString &dir) const {
  qDebug() << "Starting migration of todo list " << dir;
  QFile configFile(dir + "/config.xml");
  if (configFile.exists()) {
    if (configFile.open(QIODevice::ReadOnly)) {
      QDomDocument doc;
      QString errorMessage;
      int errorLine;
      if (doc.setContent(&configFile, &errorMessage, &errorLine)) {
        TodoList *todoList = m_targetLibrary->addTodoList(doc.documentElement().attribute("name"));
        if (todoList) {
          importTodos(todoList, dir + "/todos");
        } else {
          qWarning() << "Failed to create todo list!";
        }
      } else {
        qWarning().noquote().nospace()
            << "Failed to parse todo list config file " << configFile.fileName() << ": "
            << errorMessage << " in line " << errorLine;
      }
      configFile.close();
    } else {
      qWarning().noquote().nospace()
          << "Failed to open todo list config file " << configFile.fileName() << ": "
          << configFile.errorString();
    }
  }
  qDebug() << "Finished migration of todo list " << dir;
}

void Migrator_1_x_to_2_x::importTodos(TodoList *todoList, const QString &path) const {
  qDebug() << "Starting migration of todos in " << path;
  QDir dir(path);
  if (dir.exists()) {
    for (QString entry : dir.entryList({"*.xml"}, QDir::Files)) {
      importTodo(todoList, dir.absoluteFilePath(entry));
    }
  }
  qDebug() << "Finished migration of todos in " << path;
}

void Migrator_1_x_to_2_x::importTodo(TodoList *todoList, const QString &fileName) const {
  qDebug() << "Starting migration of todo " << fileName;
  QFile file(fileName);
  if (file.open(QIODevice::ReadOnly)) {
    QDomDocument doc;
    QString errorMessage;
    int errorLine;
    if (doc.setContent(&file, &errorMessage, &errorLine)) {
      QDomElement root = doc.documentElement();
      QString title = root.attribute("title");
      Todo *todo = todoList->addTodo(title);
      if (todo) {
        QString description = root.firstChildElement("description").text();
        todo->setNotes(description);
        todo->setDone(root.attribute("done", "false") == "true");
        QFileInfo fi(fileName);
        importTasks(todo, fi.absolutePath() + "/" + fi.baseName());    
      } else {
        qWarning() << "Failed to create todo!";
      }
    } else {
      qWarning().noquote().nospace()
          << "Failed to parse todo config " << fileName << ": " 
          << errorMessage << " on line " << errorLine;
    }
    file.close();
  } else {
    qWarning().noquote().nospace()
        << "Failed to open todo config " << fileName << ": " << file.errorString();
  }
  qDebug() << "Starting migration of todo " << fileName;
}

void Migrator_1_x_to_2_x::importTasks(Todo *todo, const QString &path) const {
  qDebug() << "Starting migration of tasks in " << path;
  QDir dir(path);
  if (dir.exists()) {
    for (QString entry : dir.entryList({"*.xml"}, QDir::Files)) {
      importTask(todo, dir.absoluteFilePath(entry));
    }
  }
  qDebug() << "Finished migration of tasks in " << path;
}

void Migrator_1_x_to_2_x::importTask(Todo *todo, const QString &fileName) const {
  qDebug() << "Starting migration of task " << fileName;
  QFile file(fileName);
  if (file.open(QIODevice::ReadOnly)) {
    QDomDocument doc;
    QString errorMessage;
    int errorLine;
    if (doc.setContent(&file, &errorMessage, &errorLine)) {
      QDomElement root = doc.documentElement();
      Task *task = todo->addTask(root.attribute("title"));
      if (task) {
        task->setDone(root.attribute("done", "false") == "true");
      } else {
        qWarning() << "Failed to create task!";
      }
    } else {
      qWarning().noquote().nospace()
          << "Failed to parse task config " << fileName << ": "
          << errorMessage << " on line " << errorLine;
    }
    file.close();
  } else {
    qWarning().noquote().nospace()
        << "Failed to open task config " << fileName << ": "
        << file.errorString();
  }
  qDebug() << "Finished migration of task " << fileName;
}
