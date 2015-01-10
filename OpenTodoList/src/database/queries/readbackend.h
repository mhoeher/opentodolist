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

#ifndef OPENTODOLIST_DATABASE_QUERIES_READBACKEND_H
#define OPENTODOLIST_DATABASE_QUERIES_READBACKEND_H

#include "datamodel/backend.h"

#include "database/storagequery.h"

#include <QList>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

/**
   @brief A query for reading DataModel::Backend objects from the Database

   This class implements a query to get DataModel::Backend objects from the Database.
 */
class ReadBackend : public StorageQuery
{
    Q_OBJECT
public:
    explicit ReadBackend();

    QList<DataModel::Backend*> backends() const;

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args, int &options ) override;
    void recordAvailable(const QVariantMap &record) override;
    void endRun() override;

signals:

    /**
       @brief A backend has been read

       This signal is emitted for every @p backend read from the database. Use Backend::fromVariant()
       to get a Backend object from the variant.
     */
    void readBackend( const QVariant &backend );

public slots:

private:
    QList< DataModel::Backend* >    m_backends;
    DataModel::Backend             *m_currentBackend;
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READBACKEND_H
