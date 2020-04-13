/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DATAMODEL_TODOLIST_H_
#define DATAMODEL_TODOLIST_H_

#include <QObject>
#include <QPointer>

#include "toplevelitem.h"

class Library;
class Todo;

class TodoList : public TopLevelItem
{
    Q_OBJECT

public:
    explicit TodoList(QObject *parent = nullptr);
    explicit TodoList(const QString &filename, QObject *parent = nullptr);
    explicit TodoList(const QDir &dir, QObject *parent = nullptr);
    virtual ~TodoList();

signals:

public slots:

protected:
private:
private slots:
};

typedef QSharedPointer<TodoList> TodoListPtr;

#endif // DATAMODEL_TODOLIST_H_
