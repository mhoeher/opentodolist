#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include "models/private/objectmodel.h"

#include "core/opentodolistinterfaces.h"
#include "datamodel/todolist.h"

#include "database/database.h"

#include <QAbstractListModel>
#include <QPointer>
#include <QSet>
#include <QVariantMap>
#include <QVector>

namespace OpenTodoList {

namespace Models {

using namespace Models::Private;
using namespace DataModel;
using namespace DataBase;

class TodoListModel : public ObjectModel
{
    Q_OBJECT

public:

    explicit TodoListModel(QObject *parent = 0);
    virtual ~TodoListModel();

public slots:

    // ObjectModel interface
protected:
    void connectToDatabase();
    void disconnectFromDatabase();
    StorageQuery *createQuery() const;

private:

    Database                       *m_database;
    QVector< DataModel::TodoList* > m_todoLists;

private slots:

    void addTodoList( const QVariant &todoList );
    void removeTodoList( const QVariant &todoList );


};

} /* Models */

} /* OpenTodoList */

#endif // TODOLISTMODEL_H
