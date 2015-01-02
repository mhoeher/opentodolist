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

#ifndef OPENTODOLIST_MODELS_BACKENDMODEL_H
#define OPENTODOLIST_MODELS_BACKENDMODEL_H

#include "models/private/objectmodel.h"

#include "datamodel/backend.h"

#include "database/database.h"

#include <QAbstractListModel>

namespace OpenTodoList {
namespace Models {

using namespace DataModel;
using namespace DataBase;

using namespace Models::Private;

class BackendModel : public ObjectModel
{
    Q_OBJECT
public:
    explicit BackendModel(QObject *parent = 0);

signals:

public slots:

    // ObjectModel interface
protected:
    void connectToDatabase();
    void disconnectFromDatabase();
    StorageQuery *createQuery() const;

private:

private slots:

    void addBackend( const QVariant &backend );

};

} // namespace Models
} // namespace OpenTodoList

#endif // OPENTODOLIST_MODELS_BACKENDMODEL_H
