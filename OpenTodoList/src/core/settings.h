/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013 - 2015  Martin Höher <martin@rpdev.net>
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QStringList>
#include <QVariant>

class QSettings;

namespace OpenTodoList {

namespace Core {

/**
   @brief A class to allow simple reading and writing of settings

   The Settings class is intended to be used to store general purpose data
   of the application, such as window size and position.
 */
class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QStringList groups READ groups WRITE setGroups NOTIFY groupsChanged )

public:
    explicit Settings( QObject *parent = 0 );
    explicit Settings(QStringList groups, QObject *parent = 0);
    virtual ~Settings();

    const QStringList &groups() const;

    Q_INVOKABLE void setValue( const QString &name, QVariant value );
    Q_INVOKABLE QVariant getValue( const QString &name, QVariant defaultValue = QVariant() );
    Q_INVOKABLE void deleteValue( const QString &name );
    
signals:
    
    void groupsChanged();

public slots:

    void setGroups( const QStringList& groups );

private:
    QSettings* m_settings;
    QStringList m_groups;
    
};

} /* Core */

} /* OpenTodoList */

#endif // SETTINGS_H
