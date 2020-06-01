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

#ifndef DATAMODEL_TODO_H_
#define DATAMODEL_TODO_H_

#include <QObject>
#include <QPointer>

#include "complexitem.h"

/**
 * @brief A single todo inside a todo list.
 *
 * A todo represents an item in a TodoList. Besides a title, due date and description, each todo
 * has a done state. Furthermore, a todo can have arbitrary many sub Task objects.
 */
class Todo : public ComplexItem
{
    Q_OBJECT

    Q_PROPERTY(bool done READ done WRITE setDone NOTIFY doneChanged)
    Q_PROPERTY(QUuid todoListUid READ todoListUid WRITE setTodoListUid NOTIFY todoListUidChanged)
    Q_PROPERTY(int percentageDone READ percentageDone NOTIFY percentageDoneChanged)
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    explicit Todo(const QString& filename, QObject* parent = nullptr);
    explicit Todo(QObject* parent = nullptr);
    explicit Todo(const QDir& dir, QObject* parent = nullptr);
    ~Todo() override;

    QUuid parentId() const override;

    bool done() const;
    void setDone(bool done);

    QUuid todoListUid() const;
    void setTodoListUid(const QUuid& todoListUid);

    int percentageDone() const;

    // Item interface
    void applyCalculatedProperties(const QVariantMap& properties) override;

    int progress() const;
    void setProgress(int progress);

signals:

    void doneChanged();
    void todoListUidChanged();
    void percentageDoneChanged();
    void progressChanged();

public slots:

protected:
    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;

private:
    QUuid m_todoListUid;
    int m_percentageDone;
    int m_progress;
    bool m_done;

    // ComplexItem interface
protected:
    void markItemAsDone() override;
};

typedef QSharedPointer<Todo> TodoPtr;

#endif // DATAMODEL_TODO_H_
