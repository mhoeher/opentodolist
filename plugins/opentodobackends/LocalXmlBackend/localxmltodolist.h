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


#ifndef LOCALXMLTODOLIST_H
#define LOCALXMLTODOLIST_H

#include "abstracttodolist.h"


class LocalXmlTodoList : public AbstractTodoList
{

    Q_OBJECT
    
public:
    
    explicit LocalXmlTodoList(const QString& key, const QString& type, QObject* parent = 0);
    virtual ~LocalXmlTodoList();
    
    Q_INVOKABLE virtual QObject *addTodo();
    
private slots:
    
    void save();
    void load();
    
};

#endif // LOCALXMLTODOLIST_H
