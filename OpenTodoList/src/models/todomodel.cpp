#include "models/todomodel.h"

#include "datamodel/objectinfo.h"

#include "listutils.h"
#include "datamodel/todo.h"

#include "database/databaseconnection.h"
#include "database/queries/readtodo.h"

#include <QTimer>

namespace OpenTodoList {

namespace Models {

TodoModel::TodoModel(QObject *parent) :
    ObjectModel( ObjectInfo<Todo>::classUuidProperty(), parent),
    m_todoList( 0 ),
    m_filter( QString() ),
    m_showDone( false ),
    m_maxDueDate( QDateTime() ),
    m_minDueDate( QDateTime() ),
    m_sortMode( TodoModel::SortTodoByName ),
    m_backendSortMode( TodoModel::SortTodoByName ),
    m_limitOffset( -1 ),
    m_limitCount( -1 )
{
    connect( this, &TodoModel::todoListChanged, this, &TodoModel::refresh );
    connect( this, &TodoModel::queryTypeChanged, this, &TodoModel::refresh );
    connect( this, &TodoModel::filterChanged, this, &TodoModel::refresh );
    connect( this, &TodoModel::showDoneChanged, this, &TodoModel::refresh );
    connect( this, &TodoModel::maxDueDateChanged, this, &TodoModel::refresh );
    connect( this, &TodoModel::minDueDateChanged, this, &TodoModel::refresh );

    connect( this, &TodoModel::objectAdded, [this] (QObject *object) {
      Todo *todo = dynamic_cast< Todo* >( object );
      if ( todo ) {
        this->connect( todo, &Todo::changed, [todo,this] {
          if ( this->database() ) {
            DatabaseConnection conn;
            conn.setDatabase( this->database() );
            conn.insertTodo( todo );
          }
        });
      }
    });
}

TodoModel::~TodoModel()
{
}

void TodoModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED( column );
    Q_UNUSED( order );
    // TODO: Move to generic ObjectModel class
    /*Comparator c( m_sortMode );
    for ( int i = 0; i < m_todos.size() - 1; ++i ) {
        for ( int j = i + 1; j < m_todos.size(); ++j ) {
            if ( c( m_todos[i], m_todos[j] ) > 0 ) {
                DataModel::Todo *first = m_todos[i];
                DataModel::Todo *second = m_todos[j];

                beginMoveRows( QModelIndex(), i, i, QModelIndex(), j + 1 );
                m_todos.removeAt(i);
                m_todos.insert( j, first );
                endMoveRows();

                if ( j - i > 1 ) {
                    beginMoveRows( QModelIndex(), j-1, j-1, QModelIndex(), i );
                    m_todos.removeAt(j-1);
                    m_todos.insert( i, second );
                    endMoveRows();
                }

            }
        }
    }*/
}

void TodoModel::sort()
{
    sort(0, Qt::AscendingOrder);
}

void TodoModel::connectToDatabase()
{
  connect( database(), &Database::todoChanged, this, &TodoModel::addTodo );
  connect( database(), &Database::todoDeleted, this, &TodoModel::removeTodo );
}

void TodoModel::disconnectFromDatabase()
{
  disconnect( database(), &Database::todoChanged, this, &TodoModel::addTodo );
  disconnect( database(), &Database::todoDeleted, this, &TodoModel::removeTodo );
}

StorageQuery *TodoModel::createQuery() const
{
  Queries::ReadTodo *query = new Queries::ReadTodo();
  if ( !m_todoList.isNull() ) {
    if ( m_todoList->hasId() ) {
      query->setParentId( m_todoList->id() );
    } else {
      query->setParentName( m_todoList->uuid() );
    }
  }
  query->setLimit( m_limitCount );
  query->setOffset( m_limitOffset );
  query->setMinDueDate( m_minDueDate );
  query->setMaxDueDate( m_maxDueDate );
  query->setShowDone( m_showDone );
  connect( query, &Queries::ReadTodo::readTodo, this, &TodoModel::addTodo, Qt::QueuedConnection );
  return query;
}

void TodoModel::addTodo(const QVariant &todo)
{
  addObject<Todo>(todo);
}

void TodoModel::removeTodo(const QVariant &todo)
{
  removeObject<Todo>(todo);
}

int TodoModel::limitCount() const
{
    return m_limitCount;
}

void TodoModel::setLimitCount(int limitCount)
{
    if ( m_limitCount != limitCount ) {
        m_limitCount = limitCount;
        emit limitCountChanged();
    }
}

int TodoModel::limitOffset() const
{
    return m_limitOffset;
}

void TodoModel::setLimitOffset(int limitOffset)
{
    if ( m_limitOffset != limitOffset ) {
        m_limitOffset = limitOffset;
        emit limitOffsetChanged();
    }
}

TodoModel::TodoSortMode TodoModel::backendSortMode() const
{
    return m_backendSortMode;
}

void TodoModel::setBackendSortMode(const TodoModel::TodoSortMode &backendSortMode)
{
    if ( m_backendSortMode != backendSortMode ) {
        m_backendSortMode = backendSortMode;
        emit backendSortModeChanged();
    }
}

QDateTime TodoModel::minDueDate() const
{
    return m_minDueDate;
}

void TodoModel::setMinDueDate(const QDateTime &minDueDate)
{
    if ( m_minDueDate != minDueDate ) {
        m_minDueDate = minDueDate;
        emit minDueDateChanged();
    }
}

TodoModel::TodoSortMode TodoModel::sortMode() const
{
    return m_sortMode;
}

void TodoModel::setSortMode(const TodoModel::TodoSortMode &sortMode)
{
    if ( m_sortMode != sortMode ) {
        m_sortMode = sortMode;
        sort();
        emit sortModeChanged();
    }
}

QDateTime TodoModel::maxDueDate() const
{
    return m_maxDueDate;
}

void TodoModel::setMaxDueDate(const QDateTime &maxDueDate)
{
    if ( m_maxDueDate != maxDueDate ) {
        m_maxDueDate = maxDueDate;
        emit maxDueDateChanged();
    }
}

bool TodoModel::showDone() const
{
    return m_showDone;
}

void TodoModel::setShowDone(bool showDone)
{
    if ( m_showDone != showDone ) {
        m_showDone = showDone;
        emit showDoneChanged();
    }
}

QString TodoModel::filter() const
{
    return m_filter;
}

void TodoModel::setFilter(const QString &filter)
{
    if ( m_filter != filter ) {
        m_filter = filter;
        emit filterChanged();
    }
}

DataModel::TodoList *TodoModel::todoList() const
{
    return m_todoList.data();
}

void TodoModel::setTodoList(DataModel::TodoList *todoList)
{
    if ( m_todoList != todoList ) {
        m_todoList = todoList;
        emit todoListChanged();
    }
}







TodoModel::Comparator::Comparator(TodoSortMode sortMode) :
    m_sortMode( sortMode )
{

}

int TodoModel::Comparator::operator ()(DataModel::Todo * const &first, DataModel::Todo * const &second) const
{
    if ( first && second ) {
        // always show open todos before done ones:
        if ( first->done() && !second->done() ) {
            return 1;
        }
        if ( second->done() && !first->done() ) {
            return -1;
        }

        // sort depending on mode
        switch ( m_sortMode ) {

        // "default" sorting ;) Applied everywhere
        case SortTodoByName:
        //    return first->title().localeAwareCompare( second->title() ) <= 0;
            break;

        case SortTodoByPriority:
            if ( first->priority() != second->priority() ) {
                return second->priority() - first->priority();
            }
            break;

        case SortTodoByDueDate:
            if ( first->dueDate() != second->dueDate() ) {
                if ( !first->dueDate().isValid() ) {
                    return 1;
                }
                if ( !second->dueDate().isValid() ) {
                    return -1;
                }
                return first->dueDate() < second->dueDate() ? -1 : 1;
            }
            break;

        case SortTodoByWeight:
            return ( first->weight() < second->weight() );
        }

        // compare everything else by title
        return first->title().localeAwareCompare( second->title() );
    }
    return 0;
}

} /* Models */

} /* OpenTodoList */
