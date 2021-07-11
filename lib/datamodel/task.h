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

#ifndef DATAMODEL_TASK_H_
#define DATAMODEL_TASK_H_

#include <QObject>

#include "item.h"

/**
 *  @brief Represents a task inside a todo.
 *
 * The Task class represents a single task inside a todo. A task simply is a line of text
 * and a flag indicating whether or not the task is done. Hence, tasks model a simple check list
 * within a todo.
 */
class Task : public Item
{
    Q_OBJECT

    Q_PROPERTY(bool done READ done WRITE setDone NOTIFY doneChanged)
    Q_PROPERTY(QUuid todoUid READ todoUid WRITE setTodoUid NOTIFY todoUidChanged)

public:
    explicit Task(QObject* parent = nullptr);
    explicit Task(const QString& filename, QObject* parent = nullptr);
    explicit Task(const QDir& dir, QObject* parent = nullptr);
    ~Task() override;

    QUuid parentId() const override;

    bool done() const;
    void setDone(bool done);

    QUuid todoUid() const;
    void setTodoUid(const QUuid& todoUid);

    Item* copyTo(const QDir& targetDirectory, const QUuid& targetLibraryUuid,
                 const QUuid& targetItemUid) override;

signals:

    void doneChanged();
    void todoUidChanged();

public slots:

private:
    bool m_done;
    QUuid m_todoUid;

protected:
    // Item interface
    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;
};

typedef QSharedPointer<Task> TaskPtr;

#endif // DATAMODEL_TASK_H_
