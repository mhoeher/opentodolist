/*
 *  OpenTodoList - A todo and task manager
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

#ifndef OPENTODOLISTINTERFACES_H
#define OPENTODOLISTINTERFACES_H

#include "opentodolistcore_global.h"
#include "todolistfactory.h"

#include <QObject>

class OPENTODOLISTCORESHARED_EXPORT OpenTodoListBackend : public QObject {
    
    Q_OBJECT
    Q_PROPERTY( QString name READ name CONSTANT )
    Q_PROPERTY( QString description READ description CONSTANT )
    Q_PROPERTY( QString type READ type CONSTANT )

public:

    explicit OpenTodoListBackend(QObject* parent = 0) : 
        QObject( parent ), 
        m_name( QString() ), 
        m_description( QString() ) {}
    virtual ~OpenTodoListBackend() {}
    virtual TodoListFactory* factory() = 0;
    
    QString name() const { return m_name; }
    QString description() const { return m_description; }
    QString type() const { return m_type; }
    
protected:
    
    void setName( const QString& name ) { m_name = name; }
    void setDescription( const QString& description ) { m_description = description; }
    void setType( const QString& type ) { m_type = type; }
    
private:
    
    QString m_name;
    QString m_description;
    QString m_type;
    
};

Q_DECLARE_INTERFACE(OpenTodoListBackend, "net.rpdev.OpenTodoList.Backend/1.0")

#endif // OPENTODOLISTINTERFACES_H
