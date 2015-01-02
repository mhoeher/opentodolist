/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
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

#ifndef OBJECTINFO_H
#define OBJECTINFO_H

#include <QMetaClassInfo>
#include <QObject>
#include <QString>
#include <QStringList>

#include <type_traits>

namespace OpenTodoList {
namespace DataModel {

/**
  @brief Template class providing meta information about classes in the data model

  This is a helper class which allows to extract type information about the classes in the
  DataModel namespace. This is used e.g. when storing and restoring such objects in the database.
 */
template<typename T>
class ObjectInfo {

public:

  /**
     @brief Returns the class name of T without leading namespace parts
   */
  static QString className() {
    QString nsName = T::staticMetaObject.className();
    QStringList parts = nsName.split("::");
    Q_ASSERT( !parts.isEmpty() );
    return parts.last();
  }

  /**
     @brief Same as className(), but with the first character in lower case.
   */
  static QString classNameLowerFirst() {
    QString className = ObjectInfo<T>::className();
    return className.mid( 0, 1 ).toLower() + className.mid( 1 );
  }

  /**
     @brief The name of the ID property of the class
   */
  static const char* classIdProperty() {
    QMetaObject mo = T::staticMetaObject;
    return mo.classInfo( mo.indexOfClassInfo( "id" ) ).value();
  }

  /**
     @brief The name of the UUID property of the class
   */
  static const char* classUuidProperty() {
    QMetaObject mo = T::staticMetaObject;
    return mo.classInfo( mo.indexOfClassInfo( "uuid" ) ).value();
  }

  /**
     @brief Returns the list of container types

     This returns the list of container types. The first entry in the list if the
     top most container type, while the last one is the direct container type holding
     objects of type T.
   */
  static QStringList containerTypes() {
    return parentTypes<typename T::ContainerType>();
  }

  /**
     @brief Returns the list of container types

     This method is equal to containerTypes() except that it returns the list of
     class names with lower cased first character.
   */
  static QStringList containerTypesLowerFirst() {
    QStringList result = containerTypes();
    for ( QString &entry : result ) {
      entry = entry.mid( 0, 1 ).toLower() + entry.mid( 1 );
    }
    return result;
  }


private:

  ObjectInfo() = delete;

  template<typename ContainerType>
  static QStringList parentTypes( typename std::enable_if< std::is_empty< ContainerType >::value, int >::type = 0 ) {
    return QStringList();
  }

  template<typename ContainerType>
  static QStringList parentTypes( typename std::enable_if< !std::is_empty< ContainerType >::value, int >::type = 0 ) {
    QStringList result = parentTypes< typename ContainerType::ContainerType >();
    result << ObjectInfo<ContainerType>::className();
    return result;
  }

};

}
}
#endif // OBJECTINFO_H

