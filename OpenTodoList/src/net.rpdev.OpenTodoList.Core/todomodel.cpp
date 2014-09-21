#include "todomodel.h"

#include "listutils.h"
#include "todolistlibrary.h"

#include <QTimer>

TodoModel::TodoModel(QObject *parent) :
    QAbstractListModel(parent),
    m_queryType( InvalidQuery ),
    m_todos(),
    m_library( 0 ),
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
    m_sortMode( Todo::SortTodoByName ),
    m_backendSortMode( Todo::SortTodoByName ),
    m_limitOffset( -1 ),
    m_limitCount( -1 )
{
    m_updateTimer->setInterval( 1000 );
    connect( m_updateTimer, SIGNAL(timeout()), this, SLOT(triggerUpdate()) );

    connect( this, SIGNAL(todoListChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(parentTodoChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(libraryChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(queryTypeChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(filterChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(showDoneChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(showDeletedChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(hideUndeletedChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(maxDueDateChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(minDueDateChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(sortModeChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(backendSortModeChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(limitOffsetChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(limitCountChanged()), this, SIGNAL(changed()) );

    connect( this, SIGNAL(changed()), this, SLOT(update()) );

    connect( this, SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SIGNAL(countChanged()) );
    connect( this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
             this, SIGNAL(countChanged()) );
}

TodoModel::~TodoModel()
{
}

TodoListLibrary *TodoModel::library() const
{
    return m_library.data();
}

void TodoModel::setLibrary(TodoListLibrary *library)
{
    if ( m_library != library ) {
        if ( m_library ) {
            disconnect( m_library->storage(), SIGNAL(todoListInserted(QString,TodoListStruct)),
                        this, SLOT(update()) );
            disconnect( m_library->storage(), SIGNAL(todoListRemoved(QString,TodoListStruct)),
                        this, SLOT(update()) );
            disconnect( m_library->storage(), SIGNAL(todoInserted(QString,TodoStruct)),
                        this, SLOT(update()) );
            disconnect( m_library->storage(), SIGNAL(todoRemoved(QString,TodoStruct)),
                        this, SLOT(update()) );
        }
        m_library = library;
        if ( m_library ) {
            connect( m_library->storage(), SIGNAL(todoListInserted(QString,TodoListStruct)),
                        this, SLOT(update()) );
            connect( m_library->storage(), SIGNAL(todoListRemoved(QString,TodoListStruct)),
                        this, SLOT(update()) );
            connect( m_library->storage(), SIGNAL(todoInserted(QString,TodoStruct)),
                        this, SLOT(update()) );
            connect( m_library->storage(), SIGNAL(todoRemoved(QString,TodoStruct)),
                        this, SLOT(update()) );
        }
        emit libraryChanged();
    }
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
            return QVariant::fromValue< Todo* >(
                        m_todos.at( index.row() ) );
        }
    }
    return QVariant();
}

void TodoModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED( column );
    Q_UNUSED( order );
    Todo::Comparator c( m_sortMode );
    for ( int i = 0; i < m_todos.size() - 1; ++i ) {
        for ( int j = i + 1; j < m_todos.size(); ++j ) {
            if ( c( m_todos[i], m_todos[j] ) > 0 ) {
                Todo *first = m_todos[i];
                Todo *second = m_todos[j];

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

/**
   @brief The type of content this model displays
 */
TodoModel::QueryType TodoModel::queryType() const
{
    return m_queryType;
}

/**
   @brief Sets what is shown by the model
 */
void TodoModel::setQueryType(const QueryType &queryType)
{
    if ( m_queryType != queryType ) {
        m_queryType = queryType;
        emit queryTypeChanged();
    }
}


/**
   @brief Updates the model

   This refreshes the model after updating the query properties of the model. Usually, it should
   not be necessary to call this manually.
 */
void TodoModel::update()
{
    m_updateTimer->stop();
    m_updateTimer->start();
}

void TodoModel::sort()
{
    sort(0, Qt::AscendingOrder);
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

Todo::TodoSortMode TodoModel::backendSortMode() const
{
    return m_backendSortMode;
}

void TodoModel::setBackendSortMode(const Todo::TodoSortMode &backendSortMode)
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

Todo::TodoSortMode TodoModel::sortMode() const
{
    return m_sortMode;
}

void TodoModel::setSortMode(const Todo::TodoSortMode &sortMode)
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

Todo *TodoModel::get(int index) const
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


Todo *TodoModel::parentTodo() const
{
    return m_parentTodo.data();
}

void TodoModel::setParentTodo(Todo *parentTodo)
{
    if ( m_parentTodo != parentTodo ) {
        m_parentTodo = parentTodo;
        emit parentTodoChanged();
    }
}

TodoList *TodoModel::todoList() const
{
    return m_todoList.data();
}

void TodoModel::setTodoList(TodoList *todoList)
{
    if ( m_todoList != todoList ) {
        m_todoList = todoList;
        emit todoListChanged();
    }
}


QString TodoModel::idForTodo(const QString &backend, const QString &todoId)
{
    return backend + "." + todoId;
}

void TodoModel::triggerUpdate()
{
    m_updateTimer->stop();

    if ( m_queryType != InvalidQuery ) {
        TodoStorageQuery *query = new TodoStorageQuery();
        query->setQueryType( m_queryType );
        query->setFilter( m_filter );
        query->setShowDone( m_showDone );
        query->setShowDeleted( m_showDeleted );
        query->setHideUndeleted( m_hideUndeleted );
        query->setMaxDueDate( m_maxDueDate );
        query->setMinDueDate( m_minDueDate );
        query->setBackendSortMode( m_backendSortMode );
        query->setLimitOffset( m_limitOffset );
        query->setLimitCount( m_limitCount );
        bool queryIsValid = true;

        switch ( m_queryType ) {
        case QueryTopLevelTodosInTodoList:
            if ( m_todoList ) {
                setLibrary( m_todoList->library() );
                query->setBackend( m_todoList->backend() );
                query->setTodoListId( m_todoList->id() );
            }
            break;

        case QuerySubTodosOfTodo:
            if ( m_parentTodo ) {
                setLibrary( m_parentTodo->library() );
                query->setBackend( m_parentTodo->backend() );
                query->setTodoListId( m_parentTodo->todoListId() );
                query->setParentTodoId( m_parentTodo->id() );
            }
            break;

        case QuerySearchTodos:
        case QueryFilterTodos:
            // nothing left todo 8) Just break to avoid setting queryIsValid to false
            break;

        default:
            queryIsValid = false;
            break;
        }

        if ( queryIsValid && m_library ) {
            connect( query, SIGNAL(todoAvailable(QString,TodoStruct)),
                     this, SLOT(addTodo(QString,TodoStruct)), Qt::QueuedConnection );
            connect( query, SIGNAL(finished()),
                     this, SLOT(removeExtraneousTodos()), Qt::QueuedConnection );
            m_library->storage()->runQuery( query );
        } else {
            delete query;
        }
    }
}

void TodoModel::addTodo(const QString &backend, const TodoStruct &todo)
{
    QString id = idForTodo( backend, todo.id.toString() );
    m_newLoadedTodos.insert( id );
    if ( !m_loadedTodos.contains( id ) ) {
        Todo *newTodo = new Todo( backend, todo, m_library, this );
        if ( newTodo ) {
            int index = OpenTodoList::ListUtils::findInsertIndex(
                        m_todos, newTodo, Todo::Comparator( m_sortMode ) );
            beginInsertRows( QModelIndex(), index, index );
            m_todos.insert( index, newTodo );
            endInsertRows();
            m_loadedTodos.insert( id );
            connect( newTodo, SIGNAL(destroyed(QObject*)), this, SLOT(handleTodoDeleted(QObject*)) );
            connect( newTodo, SIGNAL(changed()), this, SLOT(sort()) );
        }
    }
}

void TodoModel::removeExtraneousTodos()
{
    for ( int i = rowCount() - 1; i >= 0; --i ) {
        Todo *todo = m_todos.at( i );
        if ( todo ) {
            QString id = idForTodo( todo->backend(), todo->id() );
            if ( !m_newLoadedTodos.contains( id ) ) {
                beginRemoveRows( QModelIndex(), i, i );
                m_todos.remove( i );
                endRemoveRows();
                m_loadedTodos.remove( id );
                todo->deleteLater();
            }
        }
    }
    m_newLoadedTodos.clear();
}

void TodoModel::handleTodoDeleted(QObject *todo)
{
    int index = m_todos.indexOf( static_cast< Todo* >( todo ) );
    if ( index >= 0 && index < m_todos.size() ) {
        beginRemoveRows( QModelIndex(), index, index );
        m_todos.removeAt( index );
        endRemoveRows();
    }
}

TodoStorageQuery::TodoStorageQuery() :
    TodoListStorageQuery(),
    m_queryType( TodoModel::InvalidQuery ),
    m_backend( QString() ),
    m_todoListId( QString() ),
    m_parentTodoId( QString() ),
    m_filter( QString() ),
    m_showDone( false ),
    m_showDeleted( false ),
    m_maxDueDate( QDateTime() ),
    m_minDueDate( QDateTime() ),
    m_backendSortMode( Todo::SortTodoByName ),
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

TodoModel::QueryType TodoStorageQuery::queryType() const
{
    return m_queryType;
}

void TodoStorageQuery::setQueryType(const TodoModel::QueryType &queryType)
{
    m_queryType = queryType;
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
    case Todo::SortTodoByDueDate: orderPart = "dueDate"; break;
    case Todo::SortTodoByPercentageDone: orderPart = "progress"; break;
    case Todo::SortTodoByPriority: orderPart = "progress"; break;
    case Todo::SortTodoByName: orderPart = "title"; break;
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

    case TodoModel::QuerySubTodosOfTodo:
        query = QString( "SELECT * FROM todo WHERE todoList=:todoList AND parentTodo=:parentTodo AND backend=:backend %1 ORDER BY %3 ASC %2;" )
                .arg( filterPart.isEmpty() ? "" : " AND " + filterPart.join( " AND " ) )
                .arg( limitPart )
                .arg( orderPart );
        args.insert( "todoList", m_todoListId );
        args.insert( "parentTodo", m_parentTodoId );
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
    TodoStruct todo = todoFromRecord( record );
    emit todoAvailable( record.value( "backend" ).toString(), todo );
}

void TodoStorageQuery::endRun()
{
    emit finished();
}
Todo::TodoSortMode TodoStorageQuery::backendSortMode() const
{
    return m_backendSortMode;
}

void TodoStorageQuery::setBackendSortMode(const Todo::TodoSortMode &backendSortMode)
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
