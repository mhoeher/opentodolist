#include "todolistmodel.h"

#include <QDebug>

TodoListModel::TodoListModel(QObject *parent) :
    QAbstractListModel(parent),
    m_todoLists(),
    m_library( 0 ),
    m_newTodoLists(),
    m_loadedTodoLists()
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
    m_library = library;
    emit libraryChanged();
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
}

QString TodoListModel::todoListId(const QString &backend, const TodoListStruct &list)
{
    return backend + "." + list.id.toString();
}

QString TodoListModel::todoListId(const TodoList *list)
{
    return list->backend() + "." + list->id();
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
        }
    }
}

void TodoListModel::removeExtraLists()
{
    for ( int i = rowCount() - 1; i >= 0; --i ) {
        TodoList *list = m_todoLists.at( i );
        QString id = todoListId( list );
        if ( !m_newTodoLists.contains( id ) ) {
            emit beginRemoveRows( QModelIndex(), i, i );
            m_todoLists.removeAt( i );
            emit endRemoveRows();
            m_loadedTodoLists.remove( id );
            list->deleteLater();
        }
    }
    m_newTodoLists.clear();
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
