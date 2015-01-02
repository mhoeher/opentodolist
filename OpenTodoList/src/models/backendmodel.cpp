#include "backendmodel.h"

#include "datamodel/objectinfo.h"

#include "database/queries/readbackend.h"

namespace OpenTodoList {
namespace Models {

BackendModel::BackendModel(QObject *parent) :
    ObjectModel(ObjectInfo<Backend>::classUuidProperty(), parent)
{
  connect( this, &BackendModel::databaseChanged, this, &BackendModel::refresh );
}

void BackendModel::connectToDatabase()
{
  connect( database(), &Database::backendChanged, this, &BackendModel::addBackend );
}

void BackendModel::disconnectFromDatabase()
{
  disconnect( database(), &Database::backendChanged, this, &BackendModel::addBackend );
}

StorageQuery *BackendModel::createQuery() const
{
  Queries::ReadBackend *query = new Queries::ReadBackend();
  connect( query, &Queries::ReadBackend::readBackend,
           this, &BackendModel::addBackend, Qt::QueuedConnection );
  return query;
}

void BackendModel::addBackend(const QVariant &backend)
{
  addObject<Backend>( backend );
}


} // namespace Models
} // namespace OpenTodoList
