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
    Q_ENUMS(Status)
    Q_PROPERTY( QString id READ id CONSTANT)
    Q_PROPERTY( QString name READ name CONSTANT)
    Q_PROPERTY( QString description READ description CONSTANT)
    Q_PROPERTY( Status status READ status CONSTANT )
    Q_PROPERTY( bool valid READ valid CONSTANT )
public:

    enum Status {
        Invalid,
        Stopped,
        Running
    };

    static const TodoListStruct NullTodoList;
    static const TodoStruct NullTodo;

    explicit BackendWrapper( QObject *parent = 0 );
    explicit BackendWrapper(TodoListLibrary* library,
                            BackendInterface *backend,
                            QObject *parent = 0);
    virtual ~BackendWrapper();

    Status status() const { return m_status; }
    bool valid() const { return m_status != Invalid; }

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
    Q_INVOKABLE virtual bool notifyTodoListChanged(const TodoListStruct &list);
    Q_INVOKABLE virtual bool notifyTodoChanged(const TodoStruct &todo);
    Q_INVOKABLE virtual void notifyTodoListDeleted( const TodoListStruct &list );
    Q_INVOKABLE virtual void notifyTodoDeleted( const TodoStruct &todo );
    Q_INVOKABLE virtual bool canAddTodo( const TodoListStruct &list, const TodoStruct &todo );
    Q_INVOKABLE virtual void addTodo( TodoStruct newTodo, const TodoListStruct &list, const TodoStruct &todo );
    Q_INVOKABLE virtual bool canAddTodoList();
    Q_INVOKABLE virtual void addTodoList( TodoListStruct newList );

signals:

    void statusChanged();

public slots:

    void doStart();
    void doStop();

private:

    TodoListLibrary*  m_library;
    BackendInterface* m_backend;
    Status            m_status;

    // BackendInterface interface
    virtual void setDatabase(TodoListDatabase *database);

    void setStatus( Status newStatus );

};

#endif // BACKENDWRAPPER_H
