#ifndef OPENTODOLIST_MODELS_BACKENDMODEL_H
#define OPENTODOLIST_MODELS_BACKENDMODEL_H

#include "datamodel/backend.h"

#include "database/database.h"

#include <QAbstractListModel>

namespace OpenTodoList {
namespace Models {

using namespace DataModel;
using namespace DataBase;

class BackendModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)
public:
    explicit BackendModel(QObject *parent = 0);

    Database *database() const;
    void setDatabase(Database *database);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:

    void databaseChanged();

public slots:

    void refresh();

private:

    QList<Backend*> m_backends;
    Database       *m_database;

private slots:

    void addBackend( const QVariant &backend );

};

} // namespace Models
} // namespace OpenTodoList

#endif // OPENTODOLIST_MODELS_BACKENDMODEL_H
