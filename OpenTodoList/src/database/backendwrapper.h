/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BACKENDWRAPPER_H
#define BACKENDWRAPPER_H

#include "core/opentodolistinterfaces.h"

#include <QObject>
#include <QTimer>


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
    bool deleteAccount(IAccount *account) override;
    bool deleteTodoList(ITodoList *list) override;
    bool deleteTodo(ITodo *todo) override;
    bool deleteTask(ITask *task) override;
    IAccount* createAccount() override;
    ITodoList* createTodoList() override;
    ITodo* createTodo() override;
    ITask *createTask() override;
    IAccount *getAccount(const QUuid &uuid) override;
    ITodoList *getTodoList(const QUuid &uuid) override;
    ITodo *getTodo(const QUuid &uuid) override;
    ITask *getTask(const QUuid &uuid) override;
    QList<IAccount *> getAccounts(
        QueryFlags flags = QueryAny,
        int maxAccounts = 0,
        int offset = 0 ) override;
    QList<ITodoList *> getTodoLists(
        QueryFlags flags = QueryAny,
        int maxTodoLists = 0,
        int offset = 0 ) override;
    QList<ITodo *> getTodos(
        QueryFlags flags = QueryAny,
        int maxTodos = 0,
        int offset = 0 ) override;
    QList<ITask *> getTasks(
        QueryFlags flags = QueryAny,
        int maxTasks = 0,
        int offset = 0 ) override;
    bool onAccountSaved(IAccount *account) override;
    bool onTodoListSaved(ITodoList *todoList) override;
    bool onTodoSaved(ITodo *todo) override;
    bool onTaskSaved(ITask *task) override;

    // IBackend interface
    void setLocalStorageDirectory(const QString &directory) override;
    QString name() const  override;
    QString title() const  override;
    QString description() const  override;
    QSet<Capabilities> capabilities() const override;
    bool start() override;
    bool stop() override;
    void sync() override;

    IBackend *backend() const;
    void setBackend(IBackend *backend);

signals:

    void statusChanged();

public slots:

    void doStart();
    void doStop();

private:

    Database  *m_database;
    IBackend  *m_backend;
    Status     m_status;
    QTimer    *m_syncTimer;

    // BackendInterface interface
    void setDatabase(IDatabase *database) override;

    void setStatus( Status newStatus );

};

} /* DataBase */

} /* OpenTodoList */

#endif // BACKENDWRAPPER_H
