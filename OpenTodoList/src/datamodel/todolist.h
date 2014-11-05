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

#ifndef ABSTRACTTODOLIST_H
#define ABSTRACTTODOLIST_H

#include "core/opentodolistinterfaces.h"
#include "todo.h"

#include <QList>
#include <QObject>
#include <QObjectList>
#include <QPointer>
#include <QVariant>
#include <QVariantMap>

namespace OpenTodoList {

namespace DataModel {

class TodoListLibrary;

class TodoList : public QObject, public ITodoList
{
    Q_OBJECT
    Q_PROPERTY( QUuid uuid READ uuid CONSTANT )
    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    
public:

    explicit TodoList( QObject *parent = 0 );
    virtual ~TodoList();

    // ITodoList interface
public:
    const QUuid &uuid() const override;
    void setUuid(const QUuid &uuid) override;
    const QString &name() const override;
    void setName(const QString &name) override;
    QVariantMap metaAttributes() const override;
    void setMetaAttributes( const QVariantMap &metaAttributes ) override;
    QDateTime lastModificationTime() const override;
    void setLastModificationTime(const QDateTime &dateTime) override;
    QUuid accountUuid() const;
    void setAccountUuid(const QUuid &uuid);

private:
    QUuid m_uuid;
    QString m_name;
    QVariantMap m_metaAttributes;
    QDateTime m_lastModificationTime;
    QUuid m_accountUuid;

signals:
    
    void nameChanged();
    void deletedChanged();
    void metaAttributesChanged();
    void changed();
    
public slots:

};

} /* DataModel */

} /* OpenTodoList */

#endif // ABSTRACTTODOLIST_H
