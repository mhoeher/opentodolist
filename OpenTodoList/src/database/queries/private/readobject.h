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

#ifndef READOBJECT_HPP
#define READOBJECT_HPP

#include "datamodel/objectinfo.h"

#include "database/storagequery.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {
namespace Private {

using namespace OpenTodoList::DataModel;

template<typename T>
class ReadObject : public StorageQuery
{
public:

  /**
     @brief An additional condition for the read query

     This allows to specify additional conditions for the read query. A condition consists of a
     condition string that is appended to the WHERE clause of the select statement as well as
     a list of named arguments for the query.
   */
  struct Condition {
    QString condition;
    QVariantMap arguments;
  };

  typedef QList<Condition> ConditionList;

  explicit ReadObject( const QStringList &attributes );

  // StorageQuery interface
  bool query(QString &query, QVariantMap &args, int &options) override;
  void recordAvailable(const QVariantMap &record) override;
  void endRun() override;

  QList<T*> objects() const;

  QVariant parentId() const;
  void setParentId(const QVariant &parentId);

  QVariant parentName() const;
  void setParentName(const QVariant &parentName);

  QVariant id() const;
  void setId(const QVariant &id);

  QVariant uuid() const;
  void setUuid(const QVariant &uuid);

  bool onlyModified() const;
  void setOnlyModified(bool onlyModified);

  bool onlyDeleted() const;
  void setOnlyDeleted(bool onlyDeleted);

  bool includeDeleted() const;
  void setIncludeDeleted(bool includeDeleted);

  ConditionList conditions() const;
  void setConditions(const ConditionList &conditions);
  void addCondition( const Condition &condition );

  int limit() const;
  void setLimit(int limit);

  int offset() const;
  void setOffset(int offset);

protected:

  /**
     @brief Returns "generated" additional conditions

     This method can be implemented by sub classes to return additional conditions from
     any additional members these classes might have. The contained conditions will be treated
     the very same as such returned by the conditions() method.
   */
  virtual ConditionList generatedConditions() const { return ConditionList(); }

private:

  QString                   m_baseTable;
  QString                   m_attributeNameTable;
  QString                   m_attributeValueTable;
  QString                   m_parentAttribute;
  QString                   m_parentIdAttribute;
  QStringList               m_attributes;

  T                        *m_currentObject;
  QList<T*>                 m_objects;

  QVariant                  m_id;
  QVariant                  m_uuid;
  QVariant                  m_parentId;
  QVariant                  m_parentName;
  bool                      m_onlyModified;
  bool                      m_onlyDeleted;
  bool                      m_includeDeleted;
  ConditionList             m_conditions;
  int                       m_limit;
  int                       m_offset;


};

template<typename T>
ReadObject<T>::ReadObject( const QStringList &attributes ) :
  StorageQuery(),
  m_baseTable( ObjectInfo<T>::classNameLowerFirst() ),
  m_attributeNameTable( ObjectInfo<T>::classNameLowerFirst() + "MetaAttributeName" ),
  m_attributeValueTable( ObjectInfo<T>::classNameLowerFirst() + "MetaAttribute" ),
  m_parentAttribute( ObjectInfo< typename T::ContainerType >::classNameLowerFirst() ),
  m_parentIdAttribute( ObjectInfo< typename T::ContainerType >::classUuidProperty() ),
  m_attributes( attributes ),

  m_currentObject( nullptr ),
  m_objects(),

  m_id(),
  m_uuid(),
  m_parentId(),
  m_parentName(),
  m_onlyModified( false ),
  m_onlyDeleted( false ),
  m_includeDeleted( false ),
  m_conditions(),
  m_limit( 0 ),
  m_offset( 0 )
{
}

template<typename T>
bool ReadObject<T>::query(QString &query, QVariantMap &args, int &options )
{
  Q_UNUSED( options );

  QTextStream stream( &query );
  stream << "SELECT " << m_baseTable << ".id AS id, "
         << m_baseTable << ".dirty AS dirty, "
         << m_baseTable << ".disposed AS disposed,";
  for ( const QString &attribute : m_attributes ) {
    stream << " " << m_baseTable << "." << attribute << " AS " << attribute << ",";
  }
  stream << " " << m_parentAttribute << "." << m_parentIdAttribute << " AS " << m_parentAttribute << ","
         << " " << m_attributeValueTable << ".value AS metaAttributeValue,"
         << " " << m_attributeNameTable << ".name AS metaAttributeName"
         << " FROM " << m_baseTable << " ";
  QStringList containerTypes = ObjectInfo<T>::containerTypesLowerFirst();
  if ( !containerTypes.isEmpty() ) {
    QString currentBase = m_baseTable;
    for ( int i = containerTypes.size() - 1; i >= 0; --i ) {
      QString currentParent = containerTypes.at( i );
      stream << " INNER JOIN " << currentParent
             << " ON " << currentBase << "." << currentParent << " = "
             << currentParent << ".id ";
      currentBase = currentParent;
    }
  }


  stream << " LEFT OUTER JOIN " << m_attributeValueTable
         << " ON " << m_baseTable << ".id = " << m_attributeValueTable << "." << m_baseTable
         << " LEFT OUTER JOIN " << m_attributeNameTable
         << " ON " << m_attributeNameTable << ".id = " << m_attributeValueTable << ".attributeName";

  QStringList conditions;
  if ( !m_parentId.isNull() ) {
    conditions << QString( " (%1.id = :searchParentId) ").arg( m_parentAttribute );
    args.insert( "searchParentId", m_parentId );
  }
  if ( !m_parentName.isNull() ) {
    conditions << QString( " (%1.%2 = :seachParentName) " ).arg( m_parentAttribute ).arg( m_parentIdAttribute );
    args.insert( "searchParentName", m_parentName );
  }
  if ( !m_id.isNull() ) {
    conditions << QString( " (%1.id = :searchObjectId ) " ).arg( m_baseTable );
    args.insert( "searchObjectId", m_id );
  }
  if ( !m_uuid.isNull() ) {
    conditions << QString( " (%1.uuid = :searchObjectUuid ) " ).arg( m_baseTable );
    args.insert( "searchObjectUuid", m_uuid );
  }
  if ( m_onlyModified ) {
    conditions << QString( " (%1.dirty > 0) " ).arg( m_baseTable );
  }
  if ( m_onlyDeleted ) {
    conditions << QString( " (%1.disposed) " ).arg( m_baseTable );
  } else {
    if ( !m_includeDeleted ) {
      conditions << QString( " (NOT %1.disposed) " ).arg( m_baseTable );
    }
  }
  ConditionList additionalConditions;
  additionalConditions.append( m_conditions );
  additionalConditions.append( generatedConditions() );
  for ( const Condition &condition : additionalConditions ) {
    conditions << ("(" + condition.condition + ")");
    for ( const QString &key : condition.arguments.keys() ) {
      args.insert( key, condition.arguments.value( key ) );
    }
  }
  if ( !conditions.isEmpty() ) {
    stream << " WHERE " << conditions.join( " AND " );
  }

  stream << " LIMIT " << ( m_limit > 0 ? m_limit : -1 ) << " OFFSET " << m_offset << ";";
  return true;
}

template<typename T>
void ReadObject<T>::recordAvailable(const QVariantMap &record)
{
  if ( m_currentObject && m_currentObject->id() != record.value( "id" ).toInt() ) {
    m_objects << m_currentObject;
    m_currentObject = nullptr;
  }
  if ( !m_currentObject ) {
    m_currentObject = new T( this );
    m_currentObject->setId( record.value( "id" ).toInt() );
    m_currentObject->setDirty( record.value( "dirty", 0 ).toInt() );
    m_currentObject->setDisposed( record.value( "disposed", false ).toBool() );
    for ( const QString &attribute : m_attributes ) {
      m_currentObject->setProperty( attribute.toUtf8().constData(),
                                    record.value( attribute ) );
    }
    m_currentObject->setProperty( m_parentAttribute.toUtf8().constData(),
                                  record.value( m_parentAttribute ) );
  }
  if ( record.contains( "metaAttributeName" ) && !record.value( "metaAttributeName" ).isNull() ) {
    QVariantMap metaAttributes = m_currentObject->metaAttributes();
    metaAttributes.insert( record.value( "metaAttributeName" ).toString(),
                           record.value( "metaAttributeValue" ) );
    m_currentObject->setMetaAttributes( metaAttributes );
  }
}

template<typename T>
void ReadObject<T>::endRun()
{
  if ( m_currentObject != nullptr ) {
    m_objects << m_currentObject;
    m_currentObject = nullptr;
  }
}

template<typename T>
QList<T *> ReadObject<T>::objects() const
{
  return m_objects;
}

/**
  @brief The parent for which to read objects

  If this is set, then only objects are read where the ID of the parent is
  equal to this ID. Use this if the parents ID in the database is known.

  @sa setParentId()
 */
template<typename T>
QVariant ReadObject<T>::parentId() const
{
  return m_parentId;
}

/**
  @brief Sets the parent to which to return objects for

  @sa parentId()
 */
template<typename T>
void ReadObject<T>::setParentId(const QVariant &parentId)
{
  m_parentId = parentId;
}

/**
  @brief The parent to which to return objects for

  If this is set, then only objects are read where the parentIdAttribute as specified in the
  constructor is equal to the parentName. Use this if e.g. only the parent's UUID is known but not
  its ID in the database.
 */
template<typename T>
QVariant ReadObject<T>::parentName() const
{
  return m_parentName;
}

/**
  @brief Sets the parent to which to return objects for

  @sa parentName()
 */
template<typename T>
void ReadObject<T>::setParentName(const QVariant &parentName)
{
  m_parentName = parentName;
}

/**
  @brief The ID of the object to read

  If this is valid, then the query will read only the object with the specified id.

  @sa setId()
 */
template<typename T>
QVariant ReadObject<T>::id() const
{
  return m_id;
}

/**
  @brief Sets the ID of the object to read

  @sa id()
 */
template<typename T>
void ReadObject<T>::setId(const QVariant &id)
{
  m_id = id;
}

/**
  @brief The UUID of the object to read

  If this is set, then only the object with this UUID is read from the database.

  @sa setUuid()
 */
template<typename T>
QVariant ReadObject<T>::uuid() const
{
  return m_uuid;
}

/**
  @brief Sets the UUID of the object to read

  @sa uuid()
 */
template<typename T>
void ReadObject<T>::setUuid(const QVariant &uuid)
{
  m_uuid = uuid;
}

/**
  @brief Include only modified objects

  If this is set to true, then the query will return only objects marked as
  modified. Otherwise, both modified and unmodified objects are included.

  @sa setOnlyModified()
 */
template<typename T>
bool ReadObject<T>::onlyModified() const
{
  return m_onlyModified;
}

/**
  @brief Set whether only modified objects shall be returned

  @sa onlyModified()
 */
template<typename T>
void ReadObject<T>::setOnlyModified(bool onlyModified)
{
  m_onlyModified = onlyModified;
}

/**
  @brief Return objects marked for deletion

  If this is set to true, the query will read objects marked for deletion
  (i.e. with the disposed flag set to true). Otherwise, only objects with the
  flag not set are returned.

  @sa setOnlyDeleted()
 */
template<typename T>
bool ReadObject<T>::onlyDeleted() const
{
  return m_onlyDeleted;
}

/**
  @brief Sets whether only deleted objects shall be returned

  @sa onlyDeleted()
 */
template<typename T>
void ReadObject<T>::setOnlyDeleted(bool onlyDeleted)
{
  m_onlyDeleted = onlyDeleted;
}

/**
   @brief Include also deleted objects in query

   If this is set to true, the query will also return objects marked as disposed. By default,
   this is false.

   @sa setIncludeDeleted()
 */
template<typename T>
bool ReadObject<T>::includeDeleted() const
{
  return m_includeDeleted;
}

/**
   @brief Sets whether to include deleted objects in results
   @sa includeDeleted()
 */
template<typename T>
void ReadObject<T>::setIncludeDeleted(bool includeDeleted)
{
  m_includeDeleted = includeDeleted;
}

/**
  @brief A list of additional conditions

  A list of additional conditions that are appended to the list of auto generated
  conditions when reading objects. The conditions are connected with a logical and among each
  other as well as with the auto generated conditions.

  This can be used to filter on a "higher" level. Implicitly, each query will be
  joined with the parent levels. Adding conditions will allow to filter on a level above
  the "current" one by accessing the joined tables.

  @sa setConditions
 */
template<typename T>
typename ReadObject<T>::ConditionList ReadObject<T>::conditions() const
{
  return m_conditions;
}

/**
  @brief Sets additional conditions for the query

  @sa conditions()
 */
template<typename T>
void ReadObject<T>::setConditions(const ConditionList &conditions)
{
  m_conditions = conditions;
}

/**
   @brief Adds a condition

   @sa conditions()
 */
template<typename T>
void ReadObject<T>::addCondition(const typename ReadObject<T>::Condition &condition)
{
  m_conditions.append( condition );
}

/**
  @brief The maximum number of items to read

  If this is set to a value greater than zero, then no more than limit objects will be read
  from the database.

  @sa setLimit()
 */
template<typename T>
int ReadObject<T>::limit() const
{
  return m_limit;
}

/**
  @brief Sets the maximum number of objects to read

  @sa limit()
 */
template<typename T>
void ReadObject<T>::setLimit(int limit)
{
  m_limit = limit;
}

/**
  @brief Sets the offset to return objects from

  If this is greater or equal to zero, then from the list of all matching objects in the database,
  objects are returned starting at the configured offset. Otherwise, objects are returned from
  the beginning of the list (i.e. offset 0).

  @sa setOffset()
 */
template<typename T>
int ReadObject<T>::offset() const
{
  return m_offset;
}

/**
  @brief Sets the offset to return objects from

  @sa offset()
 */
template<typename T>
void ReadObject<T>::setOffset(int offset)
{
  m_offset = offset;
}

} // namespace Private
} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // READOBJECT_HPP

