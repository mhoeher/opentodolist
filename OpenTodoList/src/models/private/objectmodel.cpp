#include "objectmodel.h"

namespace OpenTodoList {
namespace Models {
namespace Private {

ObjectModel::ObjectModel(const char *uuidPropertyName, QObject *parent) :
  QAbstractListModel( parent ),
  m_database( nullptr ),
  m_objects(),
  m_uuidPropertyName(uuidPropertyName),
  m_readObjects()
{
  connect( this, &ObjectModel::databaseChanged, this, &ObjectModel::refresh );
}

ObjectModel::~ObjectModel()
{

}

int ObjectModel::rowCount(const QModelIndex &parent) const
{
  if ( parent.isValid() ) {
    return 0;
  }
  return m_objects.size();
}

QVariant ObjectModel::data(const QModelIndex &index, int role) const
{
  if ( index.isValid() && index.row() < m_objects.size() ) {
    switch ( role ) {
    case Qt::DisplayRole: return QVariant::fromValue<QObject*>( m_objects.at( index.row() ) );
    default:
      break;
    }
  }
  return QVariant();
}

Database *ObjectModel::database() const
{
  return m_database;
}

void ObjectModel::setDatabase(Database *database)
{
  if ( m_database != database ) {
    if ( m_database ) {
      disconnectFromDatabase();
    }
    m_database = database;
    if ( m_database ) {
      connectToDatabase();
    }
    emit databaseChanged();
  }
}

void ObjectModel::refresh()
{
    if ( m_database ) {
      StorageQuery *query = createQuery();
      if ( query ) {
        connect( query, &StorageQuery::queryFinished,
                 this, &ObjectModel::queryFinished, Qt::QueuedConnection );
        queryStarted();
        m_database->scheduleQuery( query );
      }
    }
}

void ObjectModel::addObject(QObject *object, int index)
{
  Q_ASSERT( object != nullptr );
  if ( index >= 0 && index < m_objects.size() ) {
    beginInsertRows( QModelIndex(), index, index );
    m_objects.insert( index, object );
    endInsertRows();
  } else {
    beginInsertRows( QModelIndex(), m_objects.size(), m_objects.size() );
    m_objects.append( object );
    endInsertRows();
  }
  connect( object, &QObject::destroyed, this, &ObjectModel::objectDestroyed );
  m_readObjects.insert( object->property( m_uuidPropertyName ).toString() );
  emit objectsChanged();
  emit objectAdded( object );
}

void ObjectModel::removeObject(QObject *object)
{
  Q_ASSERT( object != nullptr );
  for ( QObject *o : m_objects ) {
    if ( o->property( m_uuidPropertyName ) == object->property( m_uuidPropertyName ) ) {
      o->deleteLater();
    }
  }
}

QObjectList ObjectModel::objects() const
{
  return m_objects;
}

void ObjectModel::objectDestroyed(QObject *obj)
{
  int idx = m_objects.indexOf( obj );
  if ( idx >= 0 ) {
    beginRemoveRows( QModelIndex(), idx, idx );
    m_objects.removeAt( idx );
    endRemoveRows();
    emit objectsChanged();
  }
}

void ObjectModel::queryStarted()
{
  m_readObjects.clear();
}

void ObjectModel::queryFinished()
{
  for ( QObject *object : m_objects ) {
    if ( !m_readObjects.contains( object->property( m_uuidPropertyName ).toString() ) ) {
      object->deleteLater();
    }
  }
}


} // namespace Private
} // namespace Models
} // namespace OpenTodoList

