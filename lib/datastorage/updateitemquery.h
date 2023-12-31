/*
 * Copyright 2023 Martin Hoeher <martin@rpdev.net>
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

#ifndef DATASTORAGE_UPDATEITEMQUERY_H_
#define DATASTORAGE_UPDATEITEMQUERY_H_

#include <QSet>
#include <QUuid>

#include "itemsquery.h"

class Item;

class UpdateItemQuery : public ItemsQuery
{
    Q_OBJECT
    Q_PROPERTY(QUuid uid READ uid WRITE setUid NOTIFY uidChanged FINAL)
    Q_PROPERTY(QString script READ script WRITE setScript NOTIFY scriptChanged FINAL)
public:
    static const char* MarkAllTodosAndTasksAsDone;
    static const char* MarkAllTodosAndTasksAsUndone;
    explicit UpdateItemQuery(QObject* parent = nullptr);

    // ItemsQuery interface
    QUuid uid() const;
    void setUid(const QUuid& newUid);

    QString script() const;
    void setScript(const QString& newScript);

signals:
    void uidChanged();

    void scriptChanged();

protected:
    void run() override;

private:
    QUuid m_uid;
    QString m_script;
};

#endif // DATASTORAGE_UPDATEITEMQUERY_H_
