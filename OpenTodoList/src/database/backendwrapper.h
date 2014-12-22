#ifndef BACKENDWRAPPER_H
#define BACKENDWRAPPER_H

#include "core/opentodolistinterfaces.h"

#include <QObject>


namespace OpenTodoList {

namespace DataBase {

class Database;

/**
   @brief Convenience class to wrap a IBackend object

   This class is used to wrap an IBackend object and integrate it into the usual application
   infrastructure. Upon creation, the class gets an IBackend object.

   @note The wrapper does not take ownership over the IBackend. This is because
         usually backends are implemented via QObject based plugins, in which case the
         actual QObject implementing IBackend is owner by the appropriate plugin loader!
 */
class BackendWrapper :
        public QObject,
        public IDatabase,
        public IBackend
{
    Q_OBJECT
    Q_INTERFACES(OpenTodoList::IBackend)
    Q_ENUMS(Status)
    Q_PROPERTY( QString name READ name CONSTANT)
    Q_PROPERTY( QString title READ title CONSTANT)
    Q_PROPERTY( QString description READ description CONSTANT)
    Q_PROPERTY( Status status READ status CONSTANT )
    Q_PROPERTY( bool valid READ valid CONSTANT )
public:

    enum Status {
        Invalid,
        Stopped,
        Running
    };

    explicit BackendWrapper( QObject *parent = 0 );
    explicit BackendWrapper(Database* database,
                            IBackend *backend,
                            QObject *parent = 0);
    virtual ~BackendWrapper();

    Status status() const { return m_status; }
    bool valid() const { return m_status != Invalid; }

    // IDatabase interface
    bool insertAccount(IAccount *account) override;
    bool insertTodoList(ITodoList *list) override;
    bool insertTodo(ITodo *todo) override;
    bool insertTask(ITask *task) override;
    bool deleteAccount(const IAccount *account) override;
    bool deleteTodoList(const ITodoList *list) override;
    bool deleteTodo(const ITodo *todo) override;
    bool deleteTask(const ITask *task) override;
    IAccount* createAccount() override;
    ITodoList* createTodoList() override;
    ITodo* createTodo() override;
    ITask *createTask() override;
    IAccount *getAccount(const QUuid &uuid) override;
    ITodoList *getTodoList(const QUuid &uuid) override;
    ITodo *getTodo(const QUuid &uuid) override;
    ITask *getTask(const QUuid &uuid) override;
    QList<IAccount *> getModifiedAccounts(int maxAccounts) override;
    QList<ITodoList *> getModifiedTodoLists(int maxTodoLists) override;
    QList<ITodo *> getModifiedTodos(int maxTodos) override;
    QList<ITask *> getModifiedTasks(int maxTasks) override;
    bool onAccountSaved(IAccount *account) override;
    bool onTodoListSaved(ITodoList *todoList) override;
    bool onTodoSaved(ITodo *todo) override;
    bool onTaskSaved(ITask *task) override;
    QList<IAccount *> getDeletedAccounts(int maxAccounts) override;
    QList<ITodoList *> getDeletedTodoLists(int maxTodoLists) override;
    QList<ITodo *> getDeletedTodos(int maxTodos) override;
    QList<ITask *> getDeletedTasks(int maxTasks) override;

    // IBackend interface
    void setLocalStorageDirectory(const QString &directory) override;
    QString name() const  override;
    QString title() const  override;
    QString description() const  override;
    bool start() override;
    bool stop() override;

signals:

    void statusChanged();

public slots:

    void doStart();
    void doStop();

private:

    Database*  m_database;
    IBackend* m_backend;
    Status            m_status;

    // BackendInterface interface
    void setDatabase(IDatabase *database) override;

    void setStatus( Status newStatus );

};

} /* DataBase */

} /* OpenTodoList */

#endif // BACKENDWRAPPER_H
