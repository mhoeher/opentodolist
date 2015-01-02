#ifndef TODOMODEL_H
#define TODOMODEL_H

#include "models/private/objectmodel.h"

#include "core/opentodolistinterfaces.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "database/database.h"
#include "database/storagequery.h"

#include <QAbstractListModel>
#include <QPointer>
#include <QSet>
#include <QTimer>
#include <QVector>

namespace OpenTodoList {

namespace Models {

using namespace Models::Private;
using namespace DataModel;
using namespace DataBase;

/**
   @brief A model for interacting with todos

   This class implements a model on the todos in the application. It provides
   several filter capabilities to exactly specify which todos to include in the
   model. The model is inexpensive in the means that it will create
   run time objects for only the todos that are matched by the filter criteria.
 */
class TodoModel : public ObjectModel
{
    Q_OBJECT
    Q_ENUMS(TodoSortMode)
    Q_ENUMS( QueryType )
    Q_PROPERTY( OpenTodoList::DataModel::TodoList* todoList READ todoList WRITE setTodoList NOTIFY todoListChanged)
    Q_PROPERTY( QString filter READ filter WRITE setFilter NOTIFY filterChanged )
    Q_PROPERTY( bool showDone READ showDone WRITE setShowDone NOTIFY showDoneChanged )
    Q_PROPERTY( QDateTime maxDueDate READ maxDueDate WRITE setMaxDueDate NOTIFY maxDueDateChanged)
    Q_PROPERTY( QDateTime minDueDate READ minDueDate WRITE setMinDueDate NOTIFY minDueDateChanged)
    Q_PROPERTY( TodoSortMode sortMode READ sortMode WRITE setSortMode NOTIFY sortModeChanged )
    Q_PROPERTY( TodoSortMode backendSortMode READ backendSortMode WRITE setBackendSortMode NOTIFY backendSortModeChanged)
    Q_PROPERTY( int limitOffset READ limitOffset WRITE setLimitOffset NOTIFY limitOffsetChanged)
    Q_PROPERTY( int limitCount READ limitCount WRITE setLimitCount NOTIFY limitCountChanged)

public:

    enum TodoSortMode {
        SortTodoByName,
        SortTodoByPriority,
        SortTodoByDueDate,
        SortTodoByWeight
    };

    explicit TodoModel(QObject *parent = 0);
    virtual ~TodoModel();

    QString filter() const;
    void setFilter(const QString &filter);

    bool showDone() const;
    void setShowDone(bool showDone);

    QDateTime maxDueDate() const;
    void setMaxDueDate(const QDateTime &maxDueDate);

    QDateTime minDueDate() const;
    void setMinDueDate(const QDateTime &minDueDate);

    TodoSortMode sortMode() const;
    void setSortMode(const TodoSortMode &sortMode);

    TodoSortMode backendSortMode() const;
    void setBackendSortMode(const TodoSortMode &backendSortMode);

    int limitOffset() const;
    void setLimitOffset(int limitOffset);

    int limitCount() const;
    void setLimitCount(int limitCount);

    DataModel::TodoList *todoList() const;
    void setTodoList(DataModel::TodoList *todoList);

signals:

    void queryTypeChanged();
    void todoListChanged();
    void filterChanged();
    void showDoneChanged();
    void maxDueDateChanged();
    void minDueDateChanged();
    void changed();
    void sortModeChanged();
    void backendSortModeChanged();
    void limitOffsetChanged();
    void limitCountChanged();

protected:
    // ObjectModel interface
    void connectToDatabase() override;
    void disconnectFromDatabase() override;
    StorageQuery *createQuery() const override;
    bool objectFilter(QObject *object) const override;
    int compareObjects(QObject *left, QObject *right) const override;

private:

    QPointer<DataModel::TodoList> m_todoList;
    QString                       m_filter;
    bool                          m_showDone;
    QDateTime                     m_maxDueDate;
    QDateTime                     m_minDueDate;
    TodoSortMode                  m_sortMode;
    TodoSortMode                  m_backendSortMode;
    int                           m_limitOffset;
    int                           m_limitCount;

private slots:

    void addTodo( const QVariant &todo );
    void removeTodo( const QVariant &todo );


};

} /* Models */

} /* OpenTodoList */

#endif // TODOMODEL_H
