#ifndef OPENTODOLIST_MODELS_BACKENDMODEL_H
#define OPENTODOLIST_MODELS_BACKENDMODEL_H

#include "models/private/objectmodel.h"

#include "datamodel/backend.h"

#include "database/database.h"

#include <QAbstractListModel>

namespace OpenTodoList {
namespace Models {

using namespace DataModel;
using namespace DataBase;

using namespace Models::Private;

class BackendModel : public ObjectModel
{
    Q_OBJECT
public:
    explicit BackendModel(QObject *parent = 0);

signals:

public slots:

    // ObjectModel interface
protected:
    void connectToDatabase();
    void disconnectFromDatabase();
    StorageQuery *createQuery() const;

private:

private slots:

    void addBackend( const QVariant &backend );

};

} // namespace Models
} // namespace OpenTodoList

#endif // OPENTODOLIST_MODELS_BACKENDMODEL_H
