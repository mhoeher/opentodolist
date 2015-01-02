/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
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

#ifndef TODOLISTSTORAGE_H
#define TODOLISTSTORAGE_H

#include "core/opentodolistinterfaces.h"
#include "database/backendwrapper.h"
#include "pluginsloader.h"

#include <QObject>
#include <QQueue>
#include <QThread>

namespace OpenTodoList {

namespace DataBase {

class StorageQuery;
class DatabaseWorker;

/**
   @brief Provides the todo list storage for the application

   This class is responsible to provide the todo list storage for the application. Internally,
   it will start an instance of the DatabaseWorker class to maintain a SQL database which is used
   for permanent object storage. The database is well hidden. The front end uses a high level
   API to add new object to the database, update existing ones or schedule objects for permanent
   deletion from the database.

   This class also is responsible to load the various backends (i.e. plugins implementing
   the IBackend interface). Backends are used for external todo data storage, i.e. they can
   bridge from the application to any external service that allows to store todo list information.
 */
class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);
    virtual ~Database();

    void runQuery( StorageQuery *query );
    void scheduleQuery( StorageQuery *query );

signals:

    void backendChanged( const QVariant &backend );
    void accountChanged( const QVariant &account );
    void todoListChanged( const QVariant &todoList );
    void todoChanged( const QVariant &todo );
    void taskChanged( const QVariant &task );
    void accountDeleted( const QVariant &account );
    void todoListDeleted( const QVariant &todoList );
    void todoDeleted( const QVariant &todo );
    void taskDeleted( const QVariant &task );

private:

    QThread                          m_workerThread;
    DatabaseWorker                  *m_worker;
    PluginsLoader<IBackend>         *m_backendPlugins;

    QThread                          m_backendsThread;
    QVector< BackendWrapper* >       m_backends;

    BackendWrapper* backendByName( const QString &backend ) const;

#ifdef Q_OS_ANDROID
    static QString androidExtStorageLocation();
#endif
    static QString localStorageLocation( const QString &type );

private slots:

    void startBackends();

};

} /* DataBase */

} /* OpenTodoList */

#endif // TODOLISTSTORAGE_H
