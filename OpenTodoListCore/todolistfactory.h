/*
 *  OpenTodoListCore - Core functionality for the OpenTodoList application
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

#ifndef TODOLISTFACTORY_H
#define TODOLISTFACTORY_H

#include "opentodolistcore_global.h"

#include "abstracttodolist.h"

#include <QObject>

class OPENTODOLISTCORESHARED_EXPORT TodoListFactory : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString type READ type CONSTANT )
public:
    explicit TodoListFactory(const QString& type, QObject *parent = 0);

    virtual AbstractTodoList* createTodoList( QObject *parent = 0, const QString& key = QString() ) const = 0;
    
    const QString& type() const {
        return m_type;
    }
    
signals:
    
public slots:
    
private:
    
    QString m_type;
    
};

#endif // TODOLISTFACTORY_H
