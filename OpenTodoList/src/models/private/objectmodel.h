#ifndef OPENTODOLIST_MODELS_PRIVATE_OBJECTMODEL_H
#define OPENTODOLIST_MODELS_PRIVATE_OBJECTMODEL_H

#include "database/database.h"
#include "database/storagequery.h"

#include <QAbstractListModel>
#include <QObjectList>
#include <QQmlListProperty>
#include <QSet>
#include <QVariant>

namespace OpenTodoList {
namespace Models {
namespace Private {

using namespace DataBase;

class ObjectModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)
  Q_PROPERTY(QObjectList objects READ objects NOTIFY objectsChanged)
public:
  ObjectModel( const char *uuidPropertyName, QObject *parent = 0 );
  ~ObjectModel();

  // QAbstractItemModel interface
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;

  Database *database() const;
  void setDatabase(Database *database);

  QObjectList objects() const;

public slots:

  void refresh();

signals:

  void databaseChanged();
  void objectsChanged();
  void objectAdded( QObject *object );

protected:

  virtual void connectToDatabase() = 0;
  virtual void disconnectFromDatabase() = 0;
  virtual StorageQuery* createQuery() const = 0;

  void addObject( QObject *object, int index = -1 );
  void removeObject( QObject *object );

  template<typename T>
  void addObject( const QVariant &data, int index = -1 ) {
    T *tmp = new T( this );
    tmp->fromVariant( data );

    if ( tmp->disposed() ) {
      this->removeObject( tmp );
      delete tmp;
      return;
    }

    for ( QObject *o : m_objects ) {
      if ( o->property( m_uuidPropertyName ) == tmp->property( m_uuidPropertyName ) ) {
        dynamic_cast< T* >( o )->fromVariant( data );
        m_readObjects.insert( o->property( m_uuidPropertyName ).toString() );
        delete tmp;
        return;
      }
    }
    this->addObject( tmp, index );
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

private slots:

  void objectDestroyed( QObject *obj );

  void queryStarted();
  void queryFinished();

};

} // namespace Private
} // namespace Models
} // namespace OpenTodoList

#endif // OPENTODOLIST_MODELS_PRIVATE_OBJECTMODEL_H
