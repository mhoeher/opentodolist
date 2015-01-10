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

#include "objectmodel.h"

#include <QJSEngine>

namespace OpenTodoList {
namespace Models {
namespace Private {

ObjectModel::ObjectModel(const char *uuidPropertyName, QObject *parent) :
  QAbstractListModel( parent ),
  m_database( nullptr ),
  m_objects(),
  m_uuidPropertyName(uuidPropertyName),
  m_readObjects(),
  m_updateTimer(),
  m_sortTimer(),
  m_textProperty( "objectName" ),
  m_groupingFunction(QJSValue())
{
  connect( this, &ObjectModel::databaseChanged, this, &ObjectModel::refresh );

  m_updateTimer.setSingleShot( true );
  m_updateTimer.setInterval( 250 );
  connect( &m_updateTimer, &QTimer::timeout, [this] {
    if ( this->m_database ) {
      StorageQuery *query = this->createQuery();
      if ( query ) {
        connect( query, &StorageQuery::queryFinished,
                 this, &ObjectModel::queryFinished, Qt::QueuedConnection );
        this->queryStarted();
        this->m_database->scheduleQuery( query );
      }
    }
  });

  m_sortTimer.setSingleShot( true );
  m_sortTimer.setInterval( 250 );
  connect( &m_sortTimer, &QTimer::timeout, [this] { this->delayedSort(); } );
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
    case ObjectTextRole: return m_objects.at(index.row())->property(m_textProperty);
    case GroupRole: {
      if ( m_groupingFunction.isCallable() && m_groupingFunction.engine() ) {
        QJSValueList args;
        args << m_groupingFunction.engine()->toScriptValue<QObject*>( m_objects.at( index.row() ) );
        return m_groupingFunction.call( args ).toVariant();
      } else {
        return QVariant();
      }
    }
    default:
      break;
    }
  }
  return QVariant();
}

void ObjectModel::sort(int column, Qt::SortOrder order)
{
  Q_UNUSED( column );
  for ( int i = 0; i < m_objects.size() - 1; ++i ) {
    for ( int j = i + 1; j < m_objects.size(); ++j ) {
      int c = compareObjects( m_objects[i], m_objects[j] );
      if ( order == Qt::DescendingOrder ) {
        c = -c;
      }
      if ( c > 0 ) {
        QObject *first = m_objects[i];
        QObject *second = m_objects[j];

        beginMoveRows( QModelIndex(), i, i, QModelIndex(), j + 1 );
        m_objects.removeAt(i);
        m_objects.insert( j, first );
        endMoveRows();

        if ( j - i > 1 ) {
          beginMoveRows( QModelIndex(), j-1, j-1, QModelIndex(), i );
          m_objects.removeAt(j-1);
          m_objects.insert( i, second );
          endMoveRows();
        }
      }
    }
  }
}

QHash<int, QByteArray> ObjectModel::roleNames() const
{
  auto result = QAbstractListModel::roleNames();
  result.insert( ObjectTextRole, "text" );
  result.insert( GroupRole, "group" );
  return result;
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
    m_updateTimer.start();
}

/**
   @brief Removes all objects from the model
 */
void ObjectModel::clear()
{
  for ( QObject *object : m_objects ) {
    object->deleteLater();
  }
}

/**
   @brief Sorts the items in the model
 */
void ObjectModel::sort()
{
  m_sortTimer.start();
}

/**
   @brief Filter objects

   Sub classes can implement this function to have model side filtering of the objects.
   This method takes an @p object. If this method returns true, the the object will be added
   (or kept) in the model. Otherwise, the object will not be added or removed from the model.

   The default implementation returns always true, hence, no filtering takes place.
 */
bool ObjectModel::objectFilter(QObject *object) const
{
  Q_UNUSED( object );
  return true;
}

/**
   @brief Compare two objects in the model

   This is used when the sort() method of the model is invoked. Sub-classes shall reimplement this
   method in order to provide a comparison operator between objects. The method shall return a value
   less than zero in case left < right, greater than zero in case left > right or zero of both
   are equal.

   The default implementation returns zero, i.e. no sorting is applied.
 */
int ObjectModel::compareObjects(QObject *left, QObject *right) const
{
  Q_UNUSED( left );
  Q_UNUSED( right );
  return 0;
}

/**
   @brief A dynamic grouping function

   This property allows to register a custom JavaScript function in the model that is used
   to calculate a "group" value from the contained objects. This grouping can be used for
   sectioning in views in the user interface part.

   @sa setGroupingFunction
 */
QJSValue ObjectModel::groupingFunction() const
{
    return m_groupingFunction;
}

/**
   @brief Sets the grouping function
   @sa groupingFunction()
 */
void ObjectModel::setGroupingFunction(const QJSValue &groupingFunction)
{
  m_groupingFunction = groupingFunction;
  emit groupingFunctionChanged();
  if ( !m_objects.isEmpty() ) {
    emit dataChanged( index( 0 ), index( m_objects.size() - 1 ), QVector<int>() << GroupRole );
  }
}


/**
   @brief The name of the "text" property of the objects
   
   This is used to specify which propery shall be returned when
   the object is requested as "text" using the ObjectTextRole role.

   @sa setTextProperty()
 */
const char *ObjectModel::textProperty() const
{
    return m_textProperty;
}

/**
   @brief Sets the text role propery name

   @sa textProperty()
 */
void ObjectModel::setTextProperty(const char *textProperty)
{
    m_textProperty = textProperty;
}


/**
   @brief Adds an object to the model
   
   This adds the @p object to the model, inserting it at position @p index.
   If the index is either smaller than 0 or greater than the size of lists of
   already contained objects, the object will be appended to the list.

   @note The model takes ownership of the object.
 */
void ObjectModel::addObject(QObject *object, int index)
{
  Q_ASSERT( object != nullptr );
  object->setParent( this );
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

int ObjectModel::objectsCountFn(QQmlListProperty<QObject> *prop)
{
  ObjectModel *model = dynamic_cast<ObjectModel*>( prop->object );
  if ( model ) {
    return model->m_objects.size();
  }
  return 0;
}

QObject *ObjectModel::objectsAtFn(QQmlListProperty<QObject> *prop, int index)
{
  ObjectModel *model = dynamic_cast<ObjectModel*>( prop->object );
  if ( model ) {
    if ( index >= 0 && index < model->m_objects.size() ) {
      return model->m_objects.at( index );
    }
  }
  return nullptr;
}

QQmlListProperty<QObject> ObjectModel::objects()
{
  return QQmlListProperty<QObject>( this,
                                    this,
                                    &ObjectModel::objectsCountFn,
                                    &ObjectModel::objectsAtFn );
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

void ObjectModel::delayedSort()
{
  sort( 0, Qt::AscendingOrder );
}


} // namespace Private
} // namespace Models
} // namespace OpenTodoList

