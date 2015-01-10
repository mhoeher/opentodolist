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

#include "settings.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QSettings>

// TODO: Move API for getting local storage dir into core module
#include <database/database.h>

namespace OpenTodoList {

namespace Core {

Settings::Settings(QObject *parent) :
  QObject( parent ),
  m_settings(),
  m_dirty( false )
{
  QFile file( DataBase::Database::localStorageDir() + "/config.json" );
  if ( file.open( QIODevice::ReadOnly ) ) {
    QJsonDocument doc = QJsonDocument::fromJson( file.readAll() );
    m_settings = doc.toVariant().toMap();
    file.close();
  }
}

Settings::~Settings()
{
  sync();
}

/**
   @brief Saves a value

   This writes a @p value into the settings object using the given @p name. The
   value will be put in the currently set @sa groups.
 */
void Settings::setValue(const QString &name, QVariant value)
{
  m_settings.insert( name, value );
  m_dirty = true;
}

/**
   @brief Reads a value

   This will read the value with the given @p name from the settings. If the
   value is present in the currently active @sa groups its value will be
   returned. Otherwise, this method will return the @p defaultValue.
 */
QVariant Settings::getValue(const QString &name, QVariant defaultValue)
{
  return m_settings.value( name, defaultValue );
}

/**
   @brief Removes a value

   This will remove the value with the given @p name from the currently active
   @sa groups.
 */
void Settings::deleteValue(const QString &name)
{
  m_settings.remove( name );
  m_dirty = true;
}

/**
   @brief Write out current settings to disk
 */
void Settings::sync()
{
  if ( m_dirty ) {
    QJsonDocument doc = QJsonDocument::fromVariant( m_settings );
    QFile file( DataBase::Database::localStorageDir() + "/config.json" );
    if ( file.open( QIODevice::WriteOnly ) ) {
      file.write( doc.toJson() );
      file.close();
    }
    m_dirty = false;
  }
}

} /* Core */

} /* OpenTodoList */
