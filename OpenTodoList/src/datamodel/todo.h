/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013, 2014  Martin Höher <martin@rpdev.net>
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

#ifndef ABSTRACTTODO_H
#define ABSTRACTTODO_H

#include "core/opentodolistinterfaces.h"

#include <QObject>
#include <QPointer>

namespace OpenTodoList {

namespace DataModel {

class Todo : public QObject, public ITodo
{
    Q_OBJECT
    Q_PROPERTY( double weight READ weight WRITE setWeight NOTIFY weightChanged )
    Q_PROPERTY( bool isDone READ done WRITE setDone NOTIFY doneChanged )
    Q_PROPERTY( int priority READ priority WRITE setPriority NOTIFY priorityChanged )
    Q_PROPERTY( QUuid todoListUuid READ todoListUuid CONSTANT )
    Q_PROPERTY( QDateTime dueDate READ dueDate WRITE setDueDate NOTIFY dueDateChanged )
    Q_PROPERTY( QString title READ title WRITE setTitle NOTIFY titleChanged )
    Q_PROPERTY( QString description READ description WRITE setDescription NOTIFY descriptionChanged )
    Q_PROPERTY( bool isDeleted READ isDeleted WRITE setDeleted NOTIFY deletedChanged )
    Q_PROPERTY( QDateTime lastModificationTime READ lastModificationTime WRITE setLastModificationTime NOTIFY lastModificationTimeChanged)

public:

    explicit Todo( QObject* parent = 0 );
    virtual ~Todo();

    bool isNull() const;
    bool isDone() const;

    // ITodo interface
public:
    const QUuid &uuid() const override;
    void setUuid(const QUuid &uuid) override;
    double weight() const override;
    void setWeight(double weight) override;
    bool done() const override;
    void setDone(bool done) override;
    int priority() const override;
    void setPriority(int priority) override;
    const QUuid& todoListUuid() const override;
    void setTodoListUuid(const QUuid &uuid) override;
    const QDateTime &dueDate() const override;
    void setDueDate(const QDateTime &dueOn) override;
    const QString &title() const override;
    void setTitle(const QString &title) override;
    const QString &description() const override;
    void setDescription(const QString &description) override;
    bool isDeleted() const override;
    void setDeleted(bool isDeleted) override;
    QVariantMap metaAttributes() const override;
    void setMetaAttributes( const QVariantMap &metaAttributes ) override;
    QDateTime lastModificationTime() const override;
    void setLastModificationTime(const QDateTime &dateTime) override;

signals:

    void uuidChanged();
    void weightChanged();
    void doneChanged();
    void priorityChanged();
    void dueDateChanged();
    void titleChanged();
    void descriptionChanged();
    void deletedChanged();
    void lastModificationTimeChanged();

    void changed();
    void reset();

    
public slots:

    void toggle();

protected:

private:

    void setupTodo();

private:

    // ITodo properties
    QUuid                   m_uuid;
    double                  m_weight;
    bool                    m_done;
    int                     m_priority;
    QUuid                   m_todoListUuid;
    QDateTime               m_dueDate;
    QString                 m_title;
    QString                 m_description;
    bool                    m_deleted;
    QVariantMap             m_metaAttributes;
    QDateTime               m_lastModificationTime;

};

} /* DataModel */

} /* OpenTodoList */

#endif // ABSTRACTTODO_H
