#ifndef OPENTODOLIST_DATABASE_QUERIES_PRIVATE_INSERTOBJECT_H
#define OPENTODOLIST_DATABASE_QUERIES_PRIVATE_INSERTOBJECT_H

#include "database/storagequery.h"

#include <QString>
#include <QTextStream>
#include <QVariantMap>

#include <functional>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {
namespace Private {

/**
  @brief Inserts a complex object into the database

  This class provides a generic way to insert objects into the database. Complex objects
  are supposed to fulfill the following requirements:

  - They have properties which can be accessed via the Meta Object System
  - They have meta attributes which are exposed via a metaAttributes() method
  - They belong to a parent object

  This template class implements a basic protocol that will insert such objects
  into the database.
 */
template<typename T>
class InsertObject : public StorageQuery
{
public:
  explicit InsertObject(T* object,
                         const QString &baseTable,
                         const QString &attributeNameTable,
                         const QString &attributeValueTable,
                         const QStringList &attributes,
                         bool update,
                         std::function<void()> finishedCallback,
                         const QString &parentAttribute,
                         const QString &parentIdAttribute );
  ~InsertObject();

  // StorageQuery interface
  bool query(QString &query, QVariantMap &args) override;
  void newIdAvailable(const QVariant &id) override;
  void endRun() override;
  bool hasNext() const override;

private:

  enum State {
    InsertObjectState,
    InsertObjectMetaNameState,
    InsertObjectMetaValueState,
    RemoveExtraMetaValuesState,
    FinishedState
  };

  State       m_state;
  T*          m_object;
  bool        m_waitingForId;
  QString     m_baseTable;
  QString     m_attributeNameTable;
  QString     m_attributeValueTable;
  QStringList m_attributes;
  bool        m_update;
  std::function<void()> m_finishedCallback;
  QString     m_parentAttribute;
  QString     m_parentIdAttribute;

  void queryInsertObject(QTextStream &stream, QVariantMap &args );
  void queryInsertMetaName(QTextStream &stream, QVariantMap &args );
  void queryInsertMetaValue(QTextStream &stream, QVariantMap &args );
  void queryRemoveExtraMeta(QTextStream &stream, QVariantMap &args );

  void insertObjectInfo( QTextStream &stream, QVariantMap &args );


};

/**
  @brief Constructor

  Creates a new query to insert objects of type T into the database. The
  constructed query will insert the @p object into the database.
  The objects core @p attributes will be written into the @p baseTable.
  Furthermore, dynamic attributes (which can be get via the
  metaAttributes() method of the object) will be inserted into the
  @p attributeNameTable as well as @p attributeValueTable.

  If @p update is true, then the query will increase the dirty
  flag of the object and preserve the current state of the
  disposed flag. Otherwise, both flags will be reset.

  When the insertion is done, the @p finishedCallback will be invoked.

  The object's parent object is identified via the @p parentAttribute.
  It is supposed that there is a table in the database with the same name and
  which contains a value @p parentIdAttribute which is referred to by
  the objects parentAttribute property.
 */
template<typename T>
InsertObject<T>::InsertObject(T* object,
                           const QString &baseTable,
                           const QString &attributeNameTable,
                           const QString &attributeValueTable,
                           const QStringList &attributes,
                           bool update,
                           std::function<void()> finishedCallback,
                           const QString &parentAttribute,
                           const QString &parentIdAttribute) :
  StorageQuery(),
  m_state( InsertObjectState ),
  m_object( object ),
  m_waitingForId( false ),
  m_baseTable( baseTable ),
  m_attributeNameTable( attributeNameTable ),
  m_attributeValueTable( attributeValueTable ),
  m_attributes( attributes ),
  m_update( update ),
  m_finishedCallback( finishedCallback ),
  m_parentAttribute( parentAttribute ),
  m_parentIdAttribute( parentIdAttribute )
{
  Q_ASSERT( m_object != nullptr );
  Q_ASSERT( !m_baseTable.isEmpty() );
  Q_ASSERT( !m_attributeNameTable.isEmpty() );
  Q_ASSERT( !m_attributeValueTable.isEmpty() );
  Q_ASSERT( !m_parentAttribute.isEmpty() );
  Q_ASSERT( !m_parentIdAttribute.isEmpty() );
  Q_ASSERT( !m_attributes.isEmpty() );
}

template<typename T>
InsertObject<T>::~InsertObject()
{
}

template<typename T>
bool InsertObject<T>::query(QString &query, QVariantMap &args)
{
  Q_ASSERT( m_state <= RemoveExtraMetaValuesState );

  m_waitingForId = false;

  QTextStream stream( &query );

  switch ( m_state ) {

  case InsertObjectState:
  {
    queryInsertObject( stream, args );
    return true;
  }

  case InsertObjectMetaNameState:
  {
    queryInsertMetaName( stream, args );
    return true;
  }

  case InsertObjectMetaValueState:
  {
    queryInsertMetaValue( stream, args );
    return true;
  }

  case RemoveExtraMetaValuesState:
  {
    queryRemoveExtraMeta( stream, args );
    return true;
  }

  case FinishedState: return false;

  }
  return false;
}

template<typename T>
void InsertObject<T>::newIdAvailable(const QVariant &id)
{
  if ( m_waitingForId && id.isValid() ) {
    m_object->setId( id.toInt() );
  }
}

template<typename T>
void InsertObject<T>::endRun()
{
  if ( m_state == FinishedState ) {
    m_finishedCallback();
  }
}

template<typename T>
bool InsertObject<T>::hasNext() const
{
  return m_state != FinishedState;
}

template<typename T>
void InsertObject<T>::queryInsertObject(QTextStream &stream, QVariantMap &args)
{
  insertObjectInfo( stream, args );
  if ( !m_object->hasId() ) {
    m_waitingForId = true;
  }
  if ( m_update ) {
    stream << "WITH protoInfo ( dirty, disposed ) AS ( "
           << "SELECT COALESCE(dirty,0) + 1, COALESCE(disposed,false) FROM "
           << m_baseTable << " WHERE ";
    if ( m_object->hasId() ) {
      stream << "id = :searchId";
      args.insert( "searchId", m_object->id() );
    } else {
      stream << "uuid = :searchUuid2";
      args.insert( "searchUuid2", m_object->uuid() );
    }
    stream << " ) ";
  } else {
    stream << ", protoInfo ( dirty, disposed ) AS ( "
           << " VALUES ( 0, 0 ) ) ";
  }
  stream << "INSERT OR REPLACE INTO " << m_baseTable << " "
         << " ( id, dirty, disposed, " << m_parentAttribute << ", "
         << m_attributes.join( ", " ) << " ) "
         << "VALUES ( "
         << "(SELECT id FROM objectInfo), "
         << "(SELECT dirty FROM protoInfo), "
         << "(SELECT disposed FROM protoInfo), "
         << "(SELECT id FROM " << m_parentAttribute << " WHERE "
         << " " << m_parentIdAttribute  << " = :parentRef ) ";
  args.insert( "parentRef", m_object->property( m_parentAttribute.toUtf8().constData() ) );
  for ( const QString &attribute : m_attributes ) {
    stream << ", :" << attribute;
  }
  stream << " );";
  for ( const QString &attribute : m_attributes ) {
    args.insert( attribute, m_object->property( attribute.toUtf8().constData() ) );
  }
  m_state = m_object->metaAttributes().isEmpty() ?
        RemoveExtraMetaValuesState : InsertObjectMetaNameState;
}

template<typename T>
void InsertObject<T>::queryInsertMetaName(QTextStream &stream, QVariantMap &args)
{
  stream << "INSERT OR IGNORE INTO " << m_attributeNameTable << "( name ) VALUES ";
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
  m_state = InsertObjectMetaValueState;
}

template<typename T>
void InsertObject<T>::queryInsertMetaValue(QTextStream &stream, QVariantMap &args)
{
  insertObjectInfo( stream, args );
  stream << "INSERT OR REPLACE INTO " << m_attributeValueTable
         << "( " << m_baseTable << ", attributeName, value ) "
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
           << " ( SELECT id FROM " << m_attributeNameTable << " WHERE name = :" << attrName << " ), "
           << " :" << attrValue
           << ")";
    args.insert( attrName, attribute );
    args.insert( attrValue, metaAttributes.value( attribute ) );
  }
  stream << ";";
  m_state = RemoveExtraMetaValuesState;
}

template<typename T>
void InsertObject<T>::queryRemoveExtraMeta(QTextStream &stream, QVariantMap &args)
{
  insertObjectInfo( stream, args );
  QStringList metaAttributeNames = m_object->metaAttributes().keys();
  if ( metaAttributeNames.isEmpty() ) {
    stream << "DELETE FROM " << m_attributeValueTable
           << " WHERE " << m_baseTable << " = "
           << " ( SELECT id FROM  objectInfo );";
  } else {
    stream << "DELETE FROM " << m_attributeValueTable
           << " WHERE " << m_baseTable << " = ( SELECT id FROM objectInfo ) AND "
           << " attributeName NOT IN "
           << " ( SELECT id FROM " << m_attributeNameTable << " WHERE name IN (";
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
  m_state = FinishedState;
}

template<typename T>
void InsertObject<T>::insertObjectInfo(QTextStream &stream, QVariantMap &args)
{
  if ( m_object->hasId() ) {
    stream << "WITH objectInfo ( id ) AS ( VALUES ( :insertObjectInfoObjectId ) ) ";
    args.insert( "insertObjectInfoObjectId", m_object->id() );
  } else {
    stream << "WITH objectInfo ( id ) AS ( SELECT id FROM " << m_baseTable
           << " WHERE uuid = :insertObjectInfoObjectUuid ) ";
    args.insert( "insertObjectInfoObjectUuid", m_object->uuid() );
  }
}


} // namespace Private
} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_PRIVATE_INSERTOBJECT_H
