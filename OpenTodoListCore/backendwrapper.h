#ifndef BACKENDWRAPPER_H
#define BACKENDWRAPPER_H

#include "opentodolistinterfaces.h"

#include <QObject>

// forward declarations:
class TodoListLibrary;

class BackendWrapper :
        public QObject,
        public TodoListDatabase,
        public BackendInterface
{
    Q_OBJECT
    Q_INTERFACES(BackendInterface)
public:

    enum Status {
        Stopped,
        Running
    };

    explicit BackendWrapper(TodoListLibrary* library,
                            BackendInterface *backend,
                            QObject *parent = 0);
    virtual ~BackendWrapper();

    Status status() const { return m_status; }

    // TodoListDatabase interface
    virtual bool insertTodoList(const TodoListStruct &list);
    virtual bool insertTodo(const TodoStruct &todo);
    virtual bool deleteTodoList(const TodoListStruct &list);
    virtual bool deleteTodo(const TodoStruct &todo);
    virtual TodoListStruct nullTodoList();
    virtual TodoStruct nullTodo();

    // BackendInterface interface
    virtual void setLocalStorageDirectory(const QString &directory);
    virtual QString id() const;
    virtual QString name() const;
    virtual QString description() const;
    virtual bool start();
    virtual bool stop();
    virtual bool notifyTodoListChanged(const TodoListStruct &list);
    virtual bool notifyTodoChanged(const TodoStruct &todo);

signals:

public slots:

    void doStart();
    void doStop();

private:

    TodoListLibrary*  m_library;
    BackendInterface* m_backend;
    Status            m_status;

    // BackendInterface interface
    virtual void setDatabase(TodoListDatabase *database);

};

#endif // BACKENDWRAPPER_H
