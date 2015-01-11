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

#ifndef OPENTODOLIST_DATABASE_QUERIES_READTODO_H
#define OPENTODOLIST_DATABASE_QUERIES_READTODO_H

#include "datamodel/todo.h"

#include "database/queries/private/readobject.h"
#include "database/storagequery.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class ReadTodo : public ReadObject<DataModel::Todo>
{
    Q_OBJECT
public:
    explicit ReadTodo();

    QList<Todo *> todos() const;

    QDateTime minDueDate() const;
    void setMinDueDate(const QDateTime &minDueDate);

    QDateTime maxDueDate() const;
    void setMaxDueDate(const QDateTime &maxDueDate);

    bool showDone() const;
    void setShowDone(bool showDone);

    QString filter() const;
    void setFilter(const QString &filter);

    bool showOnlyScheduled() const;
    void setShowOnlyScheduled(bool showOnlyScheduled);

signals:

    void readTodo( const QVariant &todo );

public slots:

protected:
    // ReadObject interface
    ConditionList generatedConditions() const;

private:

    // Filtering
    QDateTime m_minDueDate;
    QDateTime m_maxDueDate;
    bool m_showDone;
    QString m_filter;
    bool m_showOnlyScheduled;


};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READTODO_H
