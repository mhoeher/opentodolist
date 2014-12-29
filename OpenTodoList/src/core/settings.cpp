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

#include "settings.h"

#include <QSettings>

namespace OpenTodoList {

namespace Core {

Settings::Settings(QStringList groups, QObject *parent) :
  QObject( parent ),
  m_settings( new QSettings() ),
  m_groups()
{
  setGroups( groups );
}

Settings::Settings(QObject *parent) :
  QObject( parent ),
  m_settings( new QSettings() ),
  m_groups()
{
}

Settings::~Settings()
{
  delete m_settings;
}

/**
   @brief The groups we're currently writing to

   The settings object should (at each point in time) be associated with a
   given group to which settings are written. Such a groups list could be
   [ "ApplicationViewer", "Android" ].

   @sa setGroups
 */
const QStringList &Settings::groups() const
{
  return m_groups;
}

/**
   @brief Saves a value

   This writes a @p value into the settings object using the given @p name. The
   value will be put in the currently set @sa groups.
 */
void Settings::setValue(const QString &name, QVariant value)
{
  // TODO: This hangs since updating to Qt 5.4.0 :( Check what's wrong here
  Q_UNUSED( name );
  Q_UNUSED( value );
  //m_settings->setValue( name, value );
  //m_settings->sync(); //< !Hangs!
}

/**
   @brief Reads a value

   This will read the value with the given @p name from the settings. If the
   value is present in the currently active @sa groups its value will be
   returned. Otherwise, this method will return the @p defaultValue.
 */
QVariant Settings::getValue(const QString &name, QVariant defaultValue)
{
  return m_settings->value( name, defaultValue );
}

/**
   @brief Removes a value

   This will remove the value with the given @p name from the currently active
   @sa groups.
 */
void Settings::deleteValue(const QString &name)
{
  m_settings->remove( name );
}

/**
   @brief Sets the groups to which we are writing to and reading from.
 */
void Settings::setGroups(const QStringList &groups)
{
  for ( int i = 0; i < m_groups.size(); ++i ) {
    m_settings->endGroup();
  }
  m_groups = groups;
  foreach ( QString group, m_groups ) {
    m_settings->beginGroup( group );
  }
  emit groupsChanged();
}

} /* Core */

} /* OpenTodoList */
