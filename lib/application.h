#ifndef APPLICATION_H
#define APPLICATION_H

#include "library.h"
#include "libraryfactory.h"

#include <QObject>
#include <QVector>


/**
   @brief The main class of the application
   
   The Application class is used as entry point into the OpenTodoList application. It is used
   as contained class and provides references to other objects. Basically, the Application class
   models the application, i.e. it is created when the application starts and destroyed once
   the application is to be closed.
 */
class Application : public QObject
{
  Q_OBJECT
public:
  explicit Application(QObject *parent = 0);
  
signals:
  
public slots:
  
private:
  
  Libraries               m_libraries;
  LibraryFactories        m_libraryFactories;
  
};

#endif // APPLICATION_H
