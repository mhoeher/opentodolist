#ifndef BACKENDRUNNER_H
#define BACKENDRUNNER_H

#include "backendwrapper.h"

#include <QDir>
#include <QObject>
#include <QObjectList>
#include <QStandardPaths>
#include <QThread>
#include <QVector>

// forward declarations:
class TodoListLibrary;
class BackendWrapper;

class BackendRunner : public QObject
{
    Q_OBJECT
public:
    explicit BackendRunner(TodoListLibrary *library, QObject *parent = 0);
    virtual ~BackendRunner();

    bool canAddTodo( const QString &backend, const TodoListStruct &list,
                     const TodoStruct &todo = BackendWrapper::NullTodo );
    void addTodo( const QString &backend, TodoStruct &newTodo, const TodoListStruct &list,
                  const TodoStruct &todo = BackendWrapper::NullTodo );
    bool canAddTodoList( const QString &backend );
    void addTodoList( const QString &backend, TodoListStruct newList );
    QVector<BackendWrapper *> backends() const;

signals:

    void startBackends();
    void stopBackends();

public slots:

    void start();
    void stop();
    void notifyTodoListChanged( const QString &backend, const TodoListStruct &list );
    void notifyTodoChanged( const QString &backend, const TodoStruct &todo );

private:

    QThread m_thread;
    QVector< BackendWrapper* > m_backends;

    BackendWrapper* backendByName( const QString &backend ) const;

#ifdef Q_OS_ANDROID
    static QString androidExtStorageLocation();
#endif
    static QString localStorageLocation( const QString &type );

};

#endif // BACKENDRUNNER_H
