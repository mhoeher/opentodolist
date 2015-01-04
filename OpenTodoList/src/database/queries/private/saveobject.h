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

#ifndef SAVEOBJECT_H
#define SAVEOBJECT_H

#include "database/storagequery.h"

#include "datamodel/objectinfo.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {
namespace Private {

using namespace DataModel;

/**
  @brief Generic query to mark objects as saved

  This query can be used to mark an object as saved. Execution of this query will cause the
  dirty flag in the database to be decreased by the dirty flag given in the object passed to this
  query (or set to 0 in case this would yield a negative number). Furthermore, this will update any
  meta attributes of the object. All other attributes of the object remain unsaved.
 */
template<typename T>
class SaveObject : public StorageQuery
{
public:

  explicit SaveObject( T *object );

  // StorageQuery interface
  bool query(QString &query, QVariantMap &args, int &options ) override;
  bool hasNext() const override;


private:

  enum Status {
    UpdateDirtyState,
    UpdateMetaAttributeNamesState,
    UpdateMetaAttributeValuesState,
    RemoveExtraMetaAttributeValuesState,
    FinishedState
  };

  Status m_status;
  T     *m_object;

  void insertObjectInfo( QTextStream &stream, QVariantMap &args ) const;
  void queryUpdateDirty( QTextStream &stream, QVariantMap &args );
  void queryInsertMetaNames( QTextStream &stream, QVariantMap &args );
  void queryInsertMetaValues( QTextStream &stream, QVariantMap &args );
  void queryRemoveExtraMetaValues( QTextStream &stream, QVariantMap &args );

};

template<typename T>
SaveObject<T>::SaveObject(T *object) :
  StorageQuery(),
  m_status( UpdateDirtyState ),
  m_object( object )
{
  Q_ASSERT( m_object != nullptr );
  Q_ASSERT( m_object->hasId() );
}

template<typename T>
bool SaveObject<T>::query(QString &query, QVariantMap &args, int &options)
{
  Q_UNUSED( options );
  Q_ASSERT( m_status < FinishedState );

  QTextStream stream( &query );

  switch ( m_status ) {
  case UpdateDirtyState:
    queryUpdateDirty( stream, args );
    break;
  case UpdateMetaAttributeNamesState:
    queryInsertMetaNames( stream, args );
    break;
  case UpdateMetaAttributeValuesState:
    queryInsertMetaValues( stream, args );
    break;
  case RemoveExtraMetaAttributeValuesState:
    queryRemoveExtraMetaValues( stream, args );
    break;
  default:
    return false;
  }

  return true;
}

template<typename T>
bool SaveObject<T>::hasNext() const
{
  return m_status < FinishedState;
}

template<typename T>
void SaveObject<T>::insertObjectInfo(QTextStream &stream, QVariantMap &args) const
{
  QString baseTable = ObjectInfo<T>::classNameLowerFirst();

  if ( m_object->hasId() ) {
    stream << "WITH objectInfo ( id ) AS ( VALUES ( :insertObjectInfoObjectId ) ) ";
    args.insert( "insertObjectInfoObjectId", m_object->id() );
  } else {
    stream << "WITH objectInfo ( id ) AS ( SELECT id FROM " << baseTable
           << " WHERE uuid = :insertObjectInfoObjectUuid ) ";
    args.insert( "insertObjectInfoObjectUuid", m_object->uuid() );
  }
}

template<typename T>
void SaveObject<T>::queryUpdateDirty(QTextStream &stream, QVariantMap &args)
{
  QString baseName = ObjectInfo<T>::classNameLowerFirst();
  stream << "UPDATE OR FAIL " << baseName
         << " SET dirty = MAX( 0, dirty - :decreaseValue )"
         << " WHERE id = :searchId;";
  args.insert( "decreaseValue", m_object->dirty() );
  args.insert( "searchId", m_object->id() );
  m_status = m_object->metaAttributes().size() > 0 ?
        UpdateMetaAttributeNamesState : RemoveExtraMetaAttributeValuesState;
}

template<typename T>
void SaveObject<T>::queryInsertMetaNames(QTextStream &stream, QVariantMap &args)
{
  QString attributeNameTable = ObjectInfo<T>::classNameLowerFirst() + "MetaAttributeName";

  stream << "INSERT OR IGNORE INTO " << attributeNameTable << "( name ) VALUES ";
  QStringList metaAttributeNames = m_object->metaAttributes().keys();
  for ( int i = 0; i < metaAttributeNames.size(); ++i ) {
    if ( i > 0 ) {
      stream << ", ";
    }
    QString placeholder = QString( "name%1" ).arg( i );
    stream << "( :" << placeholder << ")";
    args.insert( placeholder, metaAttributeNames.at( i ) );
  }
  stream << ";";
  m_status = UpdateMetaAttributeValuesState;
}

template<typename T>
void SaveObject<T>::queryInsertMetaValues(QTextStream &stream, QVariantMap &args)
{
  QString baseTable = ObjectInfo<T>::classNameLowerFirst();
  QString attributeValueTable = baseTable + "MetaAttribute";
  QString attributeNameTable = baseTable + "MetaAttributeName";

  insertObjectInfo( stream, args );
  stream << "INSERT OR REPLACE INTO " << attributeValueTable
         << "( " << baseTable << ", attributeName, value ) "
         << "VALUES ";
  QVariantMap metaAttributes = m_object->metaAttributes();
  QStringList metaAttributeNames = metaAttributes.keys();
  for ( int i = 0; i < metaAttributeNames.size(); ++i ) {
    QString attrName = QString( "name%1" ).arg( i );
    QString attrValue = QString( "value%1" ).arg( i );
    const QString &attribute = metaAttributeNames.at( i );
    if ( i > 0 ) {
      stream << ", ";
    }
    stream << "("
           << " ( SELECT id FROM objectInfo ), "
           << " ( SELECT id FROM " << attributeNameTable << " WHERE name = :" << attrName << " ), "
           << " :" << attrValue
           << ")";
    args.insert( attrName, attribute );
    args.insert( attrValue, metaAttributes.value( attribute ) );
  }
  stream << ";";
  m_status = RemoveExtraMetaAttributeValuesState;
}

template<typename T>
void SaveObject<T>::queryRemoveExtraMetaValues(QTextStream &stream, QVariantMap &args)
{
  QString baseTable = ObjectInfo<T>::classNameLowerFirst();
  QString attributeValueTable = baseTable + "MetaAttribute";
  QString attributeNameTable = baseTable + "MetaAttributeName";

  insertObjectInfo( stream, args );
  QStringList metaAttributeNames = m_object->metaAttributes().keys();
  if ( metaAttributeNames.isEmpty() ) {
    stream << "DELETE FROM " << attributeValueTable
           << " WHERE " << baseTable << " = "
           << " ( SELECT id FROM  objectInfo );";
  } else {
    stream << "DELETE FROM " << attributeValueTable
           << " WHERE " << baseTable << " = ( SELECT id FROM objectInfo ) AND "
           << " attributeName NOT IN "
           << " ( SELECT id FROM " << attributeNameTable << " WHERE name IN (";
    for ( int i = 0; i < metaAttributeNames.size(); ++i ) {
      QString name = QString( "name%1" ).arg( i );
      if ( i > 0 ) {
        stream << ", ";
      }
      stream << ":" << name;
      args.insert( name, metaAttributeNames.at( i ) );
    }
    stream << ") );";
  }
  m_status = FinishedState;
}

} // namespace Private
} // namespace Queries
} // namespace DataBase
} // OpenTodoList

#endif // SAVEOBJECT_H

