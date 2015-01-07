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

#ifndef OPENTODOLIST_MODELS_PRIVATE_OBJECTMODEL_H
#define OPENTODOLIST_MODELS_PRIVATE_OBJECTMODEL_H

#include "database/database.h"
#include "database/storagequery.h"

#include <QAbstractListModel>
#include <QObjectList>
#include <QQmlListProperty>
#include <QSet>
#include <QTimer>
#include <QVariant>

namespace OpenTodoList {
namespace Models {
namespace Private {

using namespace DataBase;

class ObjectModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)
  Q_PROPERTY(QQmlListProperty<QObject> objects READ objects NOTIFY objectsChanged)
public:

  enum {
    ObjectTextRole = Qt::UserRole + 1
  };

  ObjectModel( const char *uuidPropertyName, QObject *parent = 0 );
  ~ObjectModel();

  // QAbstractItemModel interface
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  void sort(int column, Qt::SortOrder order) override;
  QHash<int, QByteArray> roleNames() const override;

  Database *database() const;
  void setDatabase(Database *database);

  QQmlListProperty<QObject> objects();

  const char *textProperty() const;
  void setTextProperty(const char *textProperty);

public slots:

  void refresh();
  void clear();
  void sort();

signals:

  void databaseChanged();
  void objectsChanged();
  void objectAdded( QObject *object );

protected:

  virtual void connectToDatabase() = 0;
  virtual void disconnectFromDatabase() = 0;
  virtual StorageQuery* createQuery() const = 0;
  virtual bool objectFilter( QObject* object ) const;
  virtual int compareObjects( QObject *left, QObject *right ) const;

  template<typename T>
  void addObject( const QVariant &data, int index = -1 ) {
    T *tmp = new T( this );
    tmp->fromVariant( data );

    if ( tmp->disposed() || !objectFilter( tmp ) ) {
      this->removeObject( tmp );
      delete tmp;
      return;
    }

    for ( QObject *o : m_objects ) {
      if ( o->property( m_uuidPropertyName ) == tmp->property( m_uuidPropertyName ) ) {
        dynamic_cast< T* >( o )->fromVariant( data );
        m_readObjects.insert( o->property( m_uuidPropertyName ).toString() );
        delete tmp;
        sort();
        return;
      }
    }
    this->addObject( tmp, index );
    sort();
  }

  template<typename T>
  void removeObject( const QVariant &data ) {
    T *tmp = new T( this );
    tmp->fromVariant( data );
    for ( QObject *o : m_objects ) {
      if ( o->property( m_uuidPropertyName ) == tmp->property( m_uuidPropertyName ) ) {
        o->deleteLater();
      }
    }
    delete tmp;
  }

private:

  Database      *m_database;
  QObjectList    m_objects;
  const char    *m_uuidPropertyName;
  QSet<QString> m_readObjects;
  QTimer        m_updateTimer;
  QTimer        m_sortTimer;
  const char   *m_textProperty;

  void addObject( QObject *object, int index = -1 );
  void removeObject( QObject *object );

  static int objectsCountFn( QQmlListProperty<QObject> *prop );
  static QObject* objectsAtFn( QQmlListProperty<QObject> *prop, int index );

private slots:

  void objectDestroyed( QObject *obj );

  void queryStarted();
  void queryFinished();

  void delayedSort();
};

} // namespace Private
} // namespace Models
} // namespace OpenTodoList

#endif // OPENTODOLIST_MODELS_PRIVATE_OBJECTMODEL_H
