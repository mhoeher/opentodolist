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

#ifndef OPENTODOLIST_DATABASE_QUERIES_INSERTBACKEND_H
#define OPENTODOLIST_DATABASE_QUERIES_INSERTBACKEND_H

#include "datamodel/backend.h"

#include "database/storagequery.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace DataModel;

/**
   @brief A query to insert or update a Backend into the database

   This class implements a query which can be used to insert or update
   Backend objects into the database.
 */
class InsertBackend : public OpenTodoList::DataBase::StorageQuery
{
    Q_OBJECT
public:
    explicit InsertBackend( Backend *backend );
    virtual ~InsertBackend();

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args, int &options) override;
    void newIdAvailable(const QVariant &id) override;
    void endRun() override;
    bool hasNext() const override;

signals:

public slots:

private:
    enum State {
      InsertBackendState,
      RemoveCapabilitiesState,
      InsertCapabilitiesState,
      FinishedState
    };
    Backend *m_backend;
    State    m_state;
    bool     m_waitForId;

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_INSERTBACKEND_H
