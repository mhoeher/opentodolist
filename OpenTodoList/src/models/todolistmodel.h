#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include "core/opentodolistinterfaces.h"
#include "datamodel/todolist.h"

#include "database/database.h"

#include <QAbstractListModel>
#include <QPointer>
#include <QSet>
#include <QVariantMap>
#include <QVector>

namespace OpenTodoList {
namespace DataModel {
class TodoListLibrary;
}
}

namespace OpenTodoList {

namespace Models {

using namespace DataModel;
using namespace DataBase;

class TodoListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( int count READ rowCount NOTIFY countChanged )
    Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)

public:

    explicit TodoListModel(QObject *parent = 0);
    virtual ~TodoListModel();

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual void sort(int column, Qt::SortOrder order);

    Database *database() const;
    void setDatabase(Database *database);

signals:

    void countChanged();
    void databaseChanged();

public slots:

    void refresh();
    void sort();

private:

    Database                       *m_database;
    QVector< DataModel::TodoList* > m_todoLists;

private slots:

    void addTodoList( const QVariant &todoList );

};

} /* Models */

} /* OpenTodoList */

#endif // TODOLISTMODEL_H
