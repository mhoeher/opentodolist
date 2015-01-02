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

#include "models/modelsplugin.h"

#include "models/accountmodel.h"
#include "models/backendmodel.h"
#include "models/todolistmodel.h"
#include "models/todomodel.h"
#include "models/taskmodel.h"

#include <qqml.h>
#include <QJSEngine>

namespace OpenTodoList {

/**
 * @brief Models (implementing QAbstractItemModel) bridging into the application model
 *
 * This namespace provides several classes that make the objects defines in the
 * OpenTodoList::DataModel and stored in the OpenTodoList::DataBase available
 * in the application via a well defined interface described by QAbstractItemModel.
 */
namespace Models {

Plugin::Plugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void Plugin::registerTypes(const char *uri)
{
    //@uri net.rpdev.OpenTodoList.Models
    qmlRegisterType< AccountModel >( uri, 1, 0, "AccountModel" );
    qmlRegisterType< BackendModel >( uri, 1, 0, "BackendModel" );
    qmlRegisterType< TodoListModel >( uri, 1, 0, "TodoListModel" );
    qmlRegisterType< TodoModel >( uri, 1, 0, "TodoModel" );
    qmlRegisterType< TaskModel >( uri, 1, 0, "TaskModel" );
}

} /* Models */

} /* OpenTodoList */
