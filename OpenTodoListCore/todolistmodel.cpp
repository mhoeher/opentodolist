#include "todolistmodel.h"

#include <QDebug>
#include <QTimer>

TodoListModel::TodoListModel(QObject *parent) :
    QAbstractListModel(parent),
    m_todoLists(),
    m_library( 0 ),
    m_newTodoLists(),
    m_loadedTodoLists(),
    m_updateNeeded( false )
{
    connect( this, SIGNAL(libraryChanged()), this, SLOT(update()) );
    connect( this, SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SIGNAL(countChanged()) );
    connect( this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
             this, SIGNAL(countChanged()) );

}

TodoListModel::~TodoListModel()
{
}

TodoListLibrary *TodoListModel::library() const
{
    return m_library.data();
}

void TodoListModel::setLibrary(TodoListLibrary *library)
{
    if ( m_library != library ) {
        if ( m_library ) {
            disconnect( m_library->storage(), SIGNAL(todoListInserted(QString,TodoListStruct)),
                        this, SLOT(update()) );
            disconnect( m_library->storage(), SIGNAL(todoListRemoved(QString,TodoListStruct)),
                        this, SLOT(update()) );
        }
        m_library = library;
        if ( m_library ) {
            connect( m_library->storage(), SIGNAL(todoListInserted(QString,TodoListStruct)),
                        this, SLOT(update()) );
            connect( m_library->storage(), SIGNAL(todoListRemoved(QString,TodoListStruct)),
                        this, SLOT(update()) );
        }
        emit libraryChanged();
    }
}

int TodoListModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.isValid() ) {
        return 0;
    } else {
        return m_todoLists.size();
    }
}

QVariant TodoListModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() && index.row() < m_todoLists.size() ) {
        if ( role == Qt::DisplayRole ) {
            return QVariant::fromValue< TodoList* >(
                        m_todoLists.at( index.row() ) );
        }
    }
    return QVariant();
}

void TodoListModel::update()
{
    m_updateNeeded = true;
    QTimer::singleShot( 0, this, SLOT(triggerUpdate()) );
}

QString TodoListModel::todoListId(const QString &backend, const TodoListStruct &list)
{
    return backend + "." + list.id.toString();
}

QString TodoListModel::todoListId(const TodoList *list)
{
    return list->backend() + "." + list->id();
}

void TodoListModel::triggerUpdate()
{
    if ( !m_updateNeeded ) {
        return;
    }

    if ( m_library ) {
        emit beginUpdate();
        TodoListQuery *query = new TodoListQuery();
        connect( query, SIGNAL(addTodoList(QString,TodoListStruct)),
                 this, SLOT(addTodoList(QString,TodoListStruct)), Qt::QueuedConnection );
        connect( query, SIGNAL(finished()),
                 this, SLOT(removeExtraLists()), Qt::QueuedConnection );
        connect( query, SIGNAL(finished()),
                 this, SIGNAL(endUpdate()), Qt::QueuedConnection );
        m_library->storage()->runQuery( query );
    }

    m_updateNeeded = false;
}

void TodoListModel::addTodoList(const QString &backend, const TodoListStruct &list)
{
    QString id = todoListId( backend, list );
    m_newTodoLists.insert( id );
    if ( !m_loadedTodoLists.contains( id ) ) {
        TodoList *todoList = new TodoList( backend, list, m_library.data(), this );
        if ( todoList ) {
            emit beginInsertRows( QModelIndex(), rowCount(), rowCount() );
            m_todoLists.append( todoList );
            m_loadedTodoLists.insert( id );
            emit endInsertRows();
            connect( todoList, SIGNAL(destroyed(QObject*)), this, SLOT(handleTodoListDeleted(QObject*)) );
        }
    }
}

void TodoListModel::removeExtraLists()
{
    for ( int i = rowCount() - 1; i >= 0; --i ) {
        TodoList *list = m_todoLists.at( i );
        QString id = todoListId( list );
        if ( !m_newTodoLists.contains( id ) ) {
            beginRemoveRows( QModelIndex(), i, i );
            m_todoLists.removeAt( i );
            endRemoveRows();
            m_loadedTodoLists.remove( id );
            list->deleteLater();
        }
    }
    m_newTodoLists.clear();
}

void TodoListModel::handleTodoListDeleted(QObject *list)
{
    int index = m_todoLists.indexOf( static_cast< TodoList* >( list ) );
    if ( index >= 0 && index < m_todoLists.size() ) {
        beginRemoveRows( QModelIndex(), index, index );
        m_todoLists.removeAt( index );
        endRemoveRows();
    }
}

TodoListQuery::TodoListQuery() :
    TodoListStorageQuery()
{
}

TodoListQuery::~TodoListQuery()
{

}

void TodoListQuery::beginRun()
{
    emit clearList();
}

bool TodoListQuery::query(QString &query, QVariantMap &args)
{
    Q_UNUSED( args );
    query = "SELECT * FROM todoList ORDER BY name;";
    return true;
}

void TodoListQuery::recordAvailable(const QVariantMap &record)
{
    TodoListStruct list = todoListFromRecord( record );
    if ( !list.id.isNull() ) {
        emit addTodoList( record.value( "backend", QString() ).toString(), list );
    }
}

void TodoListQuery::endRun()
{
    emit finished();
}
