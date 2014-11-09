#include "models/todolistmodel.h"

#include "database/queries/readtodolist.h"

#include <QDebug>
#include <QTimer>

namespace OpenTodoList {

namespace Models {

TodoListModel::TodoListModel(QObject *parent) :
    QAbstractListModel(parent),
    m_todoLists(),
    m_database( nullptr )
{
    connect( this, SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SIGNAL(countChanged()) );
    connect( this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
             this, SIGNAL(countChanged()) );

}

TodoListModel::~TodoListModel()
{
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
    /*
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
    }*/
}

void TodoListModel::refresh()
{
    if ( m_database ) {
        Queries::ReadTodoList *q = new Queries::ReadTodoList();
        connect( q, &Queries::ReadTodoList::readTodoList,
                 this, &TodoListModel::addTodoList, Qt::QueuedConnection );
        m_database->scheduleQuery( q );
    }
}

void TodoListModel::sort()
{
    sort(0, Qt::AscendingOrder);
}

void TodoListModel::addTodoList(const QVariant &todoList)
{
    TodoList *tl = new TodoList( this );
    tl->fromVariant( todoList );

    // check if we already know this one and update
    for ( TodoList *existingTodoList : m_todoLists ) {
        if ( existingTodoList->uuid() == tl->uuid() ) {
            existingTodoList->fromVariant( todoList );
            delete tl;
            return;
        }
    }

    beginInsertRows( QModelIndex(), m_todoLists.size(), m_todoLists.size() );
    m_todoLists << tl;
    endInsertRows();
    emit countChanged();
}

Database *TodoListModel::database() const
{
    return m_database;
}

void TodoListModel::setDatabase(Database *database)
{
    if ( m_database != database ) {
        if ( m_database ) {
            disconnect( m_database, &Database::todoListChanged,
                        this, &TodoListModel::addTodoList );
        }
        m_database = database;
        if ( m_database ) {
            connect( m_database, &Database::todoListChanged,
                     this, &TodoListModel::addTodoList );
        }
    }
}


} /* Models */

} /* OpenTodoList */
