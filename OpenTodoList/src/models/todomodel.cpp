#include "models/todomodel.h"

#include "listutils.h"
#include "datamodel/todo.h"
#include "database/queries/readtodo.h"

#include <QTimer>

namespace OpenTodoList {

namespace Models {

TodoModel::TodoModel(QObject *parent) :
    QAbstractListModel(parent),
    m_database( nullptr ),
    m_todos(),
    m_loadedTodos(),
    m_newLoadedTodos(),
    m_updateTimer( new QTimer( this ) ),
    m_todoList( 0 ),
    m_parentTodo( 0 ),
    m_filter( QString() ),
    m_showDone( false ),
    m_showDeleted( false ),
    m_hideUndeleted( false ),
    m_maxDueDate( QDateTime() ),
    m_minDueDate( QDateTime() ),
    m_sortMode( TodoModel::SortTodoByName ),
    m_backendSortMode( TodoModel::SortTodoByName ),
    m_limitOffset( -1 ),
    m_limitCount( -1 )
{
    m_updateTimer->setInterval( 1000 );

    connect( this, &TodoModel::todoListChanged, this, &TodoModel::refilter );
    connect( this, &TodoModel::queryTypeChanged, this, &TodoModel::refilter );
    connect( this, &TodoModel::filterChanged, this, &TodoModel::refilter );
    connect( this, &TodoModel::showDoneChanged, this, &TodoModel::refilter );
    connect( this, &TodoModel::showDeletedChanged, this, &TodoModel::refilter );
    connect( this, &TodoModel::hideUndeletedChanged, this, &TodoModel::refilter );
    connect( this, &TodoModel::maxDueDateChanged, this, &TodoModel::refilter );
    connect( this, &TodoModel::minDueDateChanged, this, &TodoModel::refilter );

    connect( this, SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SIGNAL(countChanged()) );
    connect( this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
             this, SIGNAL(countChanged()) );
}

TodoModel::~TodoModel()
{
}

int TodoModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.isValid() ) {
        return 0;
    } else {
        return m_todos.size();
    }
}

QVariant TodoModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() && index.row() < m_todos.size() ) {
        if ( role == Qt::DisplayRole ) {
            return QVariant::fromValue< DataModel::Todo* >(
                        m_todos.at( index.row() ) );
        }
    }
    return QVariant();
}

void TodoModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED( column );
    Q_UNUSED( order );
    Comparator c( m_sortMode );
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
    }
}

void TodoModel::sort()
{
    sort(0, Qt::AscendingOrder);
}

void TodoModel::refresh()
{
    if ( m_database ) {
        Queries::ReadTodo *query = new Queries::ReadTodo();
        if ( !m_todoList.isNull() ) {
            query->setTodoListUuid( m_todoList->uuid() );
        }
        //query->setMinDueDate( m_minDueDate );
        //query->setMaxDueDate( m_maxDueDate );
        //query->setShowDone( m_showDone );
        //query->setShowDeleted( m_showDeleted );
        //query->setHideUndeleted( m_hideUndeleted );
        connect( query, &Queries::ReadTodo::readTodo, this, &TodoModel::addTodo, Qt::QueuedConnection );
        m_database->scheduleQuery( query );
    }
}

Database *TodoModel::database() const
{
    return m_database;
}

void TodoModel::setDatabase(Database *database)
{
    if ( m_database != database ) {
        if ( m_database ) {
            disconnect( m_database, &Database::todoChanged,
                        this, &TodoModel::addTodo );
        }
        m_database = database;
        if ( m_database ) {
            disconnect( m_database, &Database::todoChanged,
                        this, &TodoModel::addTodo );
        }
    }
}


bool TodoModel::todoIsVisible(const Todo *todo) const
{
    if ( !m_todoList.isNull() && m_todoList->uuid() != todo->todoListUuid() ) {
        return false;
    }
    if ( m_maxDueDate.isValid() && m_maxDueDate < todo->dueDate() ) {
        return false;
    }
    if ( m_minDueDate.isValid() && m_minDueDate > todo->dueDate() ) {
        return false;
    }
    if ( !m_showDone && todo->done() ) {
        return false;
    }
    return true;
}

void TodoModel::addTodo(const QVariant &todo)
{
    Todo *t = new Todo( this );
    t->fromVariant( todo );

    // check if we already have this one:
    for ( Todo *existingTodo : m_todos ) {
        if ( existingTodo->uuid() == t->uuid() ) {
            existingTodo->fromVariant( todo );
            delete t;
            return;
        }
    }

    // Append:
    if ( todoIsVisible( t ) ) {
        beginInsertRows( QModelIndex(), m_todos.size(), m_todos.size() );
        m_todos.append( t );
        endInsertRows();
    } else {
        delete t;
    }
}

void TodoModel::refilter()
{
    for ( int i = m_todos.size() - 1; i >= 0; --i ) {
        Todo* t = m_todos.at( i );
        if ( !todoIsVisible( t ) ) {
            beginRemoveRows( QModelIndex(), i, i );
            m_todos.removeAt( i );
            endRemoveRows();
            t->deleteLater();
        }
    }
    refresh();
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

bool TodoModel::hideUndeleted() const
{
    return m_hideUndeleted;
}

void TodoModel::setHideUndeleted(bool hideUndeleted)
{
    if ( m_hideUndeleted != hideUndeleted ) {
        m_hideUndeleted = hideUndeleted;
        emit hideUndeletedChanged();
    }
}

DataModel::Todo *TodoModel::get(int index) const
{
    if ( index >= 0 && index < m_todos.size() ) {
        return m_todos.at( index );
    }
    return 0;
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

bool TodoModel::showDeleted() const
{
    return m_showDeleted;
}

void TodoModel::setShowDeleted(bool showDeleted)
{
    if ( m_showDeleted != showDeleted ) {
        m_showDeleted = showDeleted;
        emit showDeletedChanged();
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






// TODO: Remove this (kept for quick reference of prev. implementation)
/*
TodoStorageQuery::TodoStorageQuery() :
    StorageQuery(),
    m_backend( QString() ),
    m_todoListId( QString() ),
    m_parentTodoId( QString() ),
    m_filter( QString() ),
    m_showDone( false ),
    m_showDeleted( false ),
    m_maxDueDate( QDateTime() ),
    m_minDueDate( QDateTime() ),
    m_backendSortMode( TodoModel::SortTodoByName ),
    m_limitOffset( -1 ),
    m_limitCount( -1 )
{
}

TodoStorageQuery::~TodoStorageQuery()
{
}

QString TodoStorageQuery::backend() const
{
    return m_backend;
}

void TodoStorageQuery::setBackend(const QString &backend)
{
    m_backend = backend;
}
QString TodoStorageQuery::todoListId() const
{
    return m_todoListId;
}

void TodoStorageQuery::setTodoListId(const QString &todoListId)
{
    m_todoListId = todoListId;
}
QString TodoStorageQuery::parentTodoId() const
{
    return m_parentTodoId;
}

void TodoStorageQuery::setParentTodoId(const QString &parentTodoId)
{
    m_parentTodoId = parentTodoId;
}

void TodoStorageQuery::beginRun()
{
    // nothing to be done here
}

bool TodoStorageQuery::query(QString &query, QVariantMap &args)
{
    QStringList filterPart;
    QString limitPart;
    QString orderPart;
    if ( !m_filter.isEmpty() ) {
        filterPart << "( title LIKE :titleFilter OR description LIKE :descriptionFilter ) ";
        args.insert( "titleFilter", "%" + m_filter + "%" );
        args.insert( "descriptionFilter", "%" + m_filter + "%" );
    }
    if ( !m_showDone ) {
        filterPart << " progress < 100 ";
    }
    if ( !m_showDeleted ) {
        filterPart << " deleted = 0 ";
    }
    if ( m_hideUndeleted ) {
        filterPart << " deleted = 1 ";
    }
    if ( m_maxDueDate.isValid() ) {
        filterPart << " dueDate IS NOT NULL AND dueDate<=:maxDueDate ";
        args.insert( "maxDueDate", m_maxDueDate );
    }
    if ( m_minDueDate.isValid() ) {
        filterPart << " dueDate IS NOT NULL AND dueDate>=:minDueDate ";
        args.insert( "minDueDate", m_minDueDate );
    }

    if ( m_limitCount > 0 ) {
        limitPart = " LIMIT " + QString::number( m_limitCount );
    }
    if ( m_limitOffset > 0 ) {
        limitPart += " OFFSET " + QString::number( m_limitOffset );
    }

    switch ( m_backendSortMode ) {
    case TodoModel::SortTodoByDueDate: orderPart = "dueDate"; break;
    case TodoModel::SortTodoByPriority: orderPart = "priority"; break;
    case TodoModel::SortTodoByName: orderPart = "title"; break;
    }


    switch ( m_queryType ) {
    case TodoModel::QueryTopLevelTodosInTodoList:
        query = QString( "SELECT * FROM todo WHERE parentTodo IS NULL AND todoList=:todoList AND backend=:backend %1 ORDER BY %3 ASC %2;" )
                .arg( filterPart.isEmpty() ? "" : " AND " + filterPart.join( " AND " ) )
                .arg( limitPart )
                .arg( orderPart );
        args.insert( "todoList", m_todoListId );
        args.insert( "backend", m_backend );
        return true;

    case TodoModel::QuerySearchTodos:
        if ( m_filter.isEmpty() ) {
            return false;
        }
        // fall through to....

    case TodoModel::QueryFilterTodos:
        query = QString( "SELECT * FROM todo WHERE %1 ORDER BY %3 ASC %2;" )
                .arg( filterPart.join( " AND " ) )
                .arg( limitPart )
                .arg( orderPart );
        return true;

    default:
        return false;
    }
}

void TodoStorageQuery::recordAvailable(const QVariantMap &record)
{
    //TodoStruct todo = todoFromRecord( record );
    //emit todoAvailable( record.value( "backend" ).toString(), todo );
}

TodoModel::TodoSortMode TodoStorageQuery::backendSortMode() const
{
    return m_backendSortMode;
}

void TodoStorageQuery::setBackendSortMode(const TodoModel::TodoSortMode &backendSortMode)
{
    m_backendSortMode = backendSortMode;
}
int TodoStorageQuery::limitOffset() const
{
    return m_limitOffset;
}

void TodoStorageQuery::setLimitOffset(int limitOffset)
{
    m_limitOffset = limitOffset;
}
int TodoStorageQuery::limitCount() const
{
    return m_limitCount;
}

void TodoStorageQuery::setLimitCount(int limitCount)
{
    m_limitCount = limitCount;
}



QDateTime TodoStorageQuery::minDueDate() const
{
    return m_minDueDate;
}

void TodoStorageQuery::setMinDueDate(const QDateTime &minDueDate)
{
    m_minDueDate = minDueDate;
}

bool TodoStorageQuery::hideUndeleted() const
{
    return m_hideUndeleted;
}

void TodoStorageQuery::setHideUndeleted(bool hideUndeleted)
{
    m_hideUndeleted = hideUndeleted;
}

bool TodoStorageQuery::showDone() const
{
    return m_showDone;
}

void TodoStorageQuery::setShowDone(bool showDone)
{
    m_showDone = showDone;
}
bool TodoStorageQuery::showDeleted() const
{
    return m_showDeleted;
}

void TodoStorageQuery::setShowDeleted(bool showDeleted)
{
    m_showDeleted = showDeleted;
}
QDateTime TodoStorageQuery::maxDueDate() const
{
    return m_maxDueDate;
}

void TodoStorageQuery::setMaxDueDate(const QDateTime &maxDueDate)
{
    m_maxDueDate = maxDueDate;
}



QString TodoStorageQuery::filter() const
{
    return m_filter;
}

void TodoStorageQuery::setFilter(const QString &filter)
{
    m_filter = filter;
}

*/

} /* Models */

} /* OpenTodoList */
