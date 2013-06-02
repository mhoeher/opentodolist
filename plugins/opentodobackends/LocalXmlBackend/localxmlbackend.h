/*
 *  LocalXmlBackend - Local XML storage for OpenTodoList
 *  Copyright (C) 2013  Martin Höher <martin@rpdev.net>
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

#ifndef LOCALXMLBACKEND_H
#define LOCALXMLBACKEND_H

#include "opentodolistinterfaces.h"
#include "todolistfactory.h"

class LocalXmlBackend : public OpenTodoListBackend
{
    Q_OBJECT
    Q_INTERFACES(OpenTodoListBackend)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "net.rpdev.OpenTodoList.Backend/1.0" FILE "LocalXmlBackend.json")
#endif // QT_VERSION >= 0x050000
    
public:
    LocalXmlBackend(QObject *parent = 0);

    TodoListFactory *factory();

private:

    TodoListFactory *m_factory;
};

#endif // LOCALXMLBACKEND_H
