#include "models/todolistmodel.h"

#include "listutils.h"

#include <QDebug>
#include <QTimer>

namespace OpenTodoList {

namespace Models {

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

DataModel::TodoListLibrary *TodoListModel::library() const
{
    return m_library.data();
}

void TodoListModel::setLibrary(DataModel::TodoListLibrary *library)
{
    if ( m_library != library ) {
        if ( m_library ) {
            /*disconnect( m_library->storage(), SIGNAL(todoListInserted(QString,TodoListStruct)),
                        this, SLOT(update()) );
            disconnect( m_library->storage(), SIGNAL(todoListRemoved(QString,TodoListStruct)),
                        this, SLOT(update()) );*/
        }
        m_library = library;
        if ( m_library ) {
            /*connect( m_library->storage(), SIGNAL(todoListInserted(QString,TodoListStruct)),
                     this, SLOT(update()) );
            connect( m_library->storage(), SIGNAL(todoListRemoved(QString,TodoListStruct)),
                     this, SLOT(update()) );*/
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
            return QVariant::fromValue< DataModel::TodoList* >(
                        m_todoLists.at( index.row() ) );
        }
    }
    return QVariant();
}

void TodoListModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED( column );
    Q_UNUSED( order );
    Comparator c;
    for ( int i = 0; i < m_todoLists.size() - 1; ++i ) {
        for ( int j = i + 1; j < m_todoLists.size(); ++j ) {
            qDebug() << m_todoLists[i]->name() << "<" << m_todoLists[j]->name();
            if ( c( m_todoLists[i], m_todoLists[j] ) > 0 ) {
                qDebug() << "False";
                auto *first = m_todoLists[i];
                auto *second = m_todoLists[j];

                beginMoveRows( QModelIndex(), i, i, QModelIndex(), j + 1 );
                m_todoLists.removeAt(i);
                m_todoLists.insert( j, first );
                endMoveRows();

                if ( j - i > 1 ) {
                    beginMoveRows( QModelIndex(), j-1, j-1, QModelIndex(), i );
                    m_todoLists.removeAt(j-1);
                    m_todoLists.insert( i, second );
                    endMoveRows();
                }

            }
        }
    }
}

void TodoListModel::update()
{
    m_updateNeeded = true;
    QTimer::singleShot( 0, this, SLOT(triggerUpdate()) );
}

void TodoListModel::sort()
{
    sort(0, Qt::AscendingOrder);
}

void TodoListModel::triggerUpdate()
{
    if ( !m_updateNeeded ) {
        return;
    }

    if ( m_library ) {
        /*
        emit beginUpdate();
        TodoListQuery *query = new TodoListQuery();
        connect( query, SIGNAL(addTodoList(QString,TodoListStruct)),
                 this, SLOT(addTodoList(QString,TodoListStruct)), Qt::QueuedConnection );
        connect( query, SIGNAL(finished()),
                 this, SLOT(removeExtraLists()), Qt::QueuedConnection );
        connect( query, SIGNAL(finished()),
                 this, SIGNAL(endUpdate()), Qt::QueuedConnection );
        m_library->storage()->runQuery( query );
        */
    }

    m_updateNeeded = false;
}

//void TodoListModel::addTodoList(const QString &backend, const TodoListStruct &list)
//{
    // TODO: Implement me!
    /*
    QString id = todoListId( backend, list );
    m_newTodoLists.insert( id );
    if ( !m_loadedTodoLists.contains( id ) ) {
        auto *todoList = new DataModel::TodoList( backend, list, m_library.data(), this );
        if ( todoList ) {
            int index = OpenTodoList::ListUtils::findInsertIndex( m_todoLists, todoList, DataModel::TodoList::Comparator() );
            emit beginInsertRows( QModelIndex(), index, index );
            m_todoLists.insert( index, todoList );
            m_loadedTodoLists.insert( id );
            emit endInsertRows();
            connect( todoList, SIGNAL(destroyed(QObject*)), this, SLOT(handleTodoListDeleted(QObject*)) );
            connect( todoList, SIGNAL(changed()), this, SLOT(sort()) );
        }
    }
    */
//}

void TodoListModel::removeExtraLists()
{
    for ( int i = rowCount() - 1; i >= 0; --i ) {
        auto list = m_todoLists.at( i );
        QString id = list->uuid().toString();
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
    int index = m_todoLists.indexOf( static_cast< DataModel::TodoList* >( list ) );
    if ( index >= 0 && index < m_todoLists.size() ) {
        beginRemoveRows( QModelIndex(), index, index );
        m_todoLists.removeAt( index );
        endRemoveRows();
    }
}

TodoListQuery::TodoListQuery() :
    StorageQuery()
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
}

void TodoListQuery::endRun()
{
    emit finished();
}


















/**
   @brief Compares two TodoList objects

   Returns a value less than, equal to or greater than 0 depending whether the
   names of the todo lists are less than, equal to or greater than.
 */
int TodoListModel::Comparator::operator ()(DataModel::TodoList * const &first,
                                           DataModel::TodoList * const &second) const {
    if ( first && second ) {
        return first->name().localeAwareCompare( second->name() );
    }
    return 0;
}




} /* Models */

} /* OpenTodoList */
