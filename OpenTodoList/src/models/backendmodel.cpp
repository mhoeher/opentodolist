#include "backendmodel.h"

#include "database/queries/readbackend.h"

namespace OpenTodoList {
namespace Models {

BackendModel::BackendModel(QObject *parent) :
    QAbstractListModel(parent)
{
}
Database *BackendModel::database() const
{
    return m_database;
}

void BackendModel::setDatabase(Database *database)
{
    if ( m_database != database ) {
        if ( m_database ) {
            disconnect( m_database, &Database::backendChanged, this, &BackendModel::addBackend );
        }
        m_database = database;
        if ( m_database ) {
            connect( m_database, &Database::backendChanged, this, &BackendModel::addBackend );
        }
        emit databaseChanged();
    }
}

int BackendModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.isValid() ) {
        return 0;
    }
    return m_backends.size();
}

QVariant BackendModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() && index.row() < m_backends.size() ) {
        switch ( role ) {
        case Qt::DisplayRole: return QVariant::fromValue< QObject* >( m_backends.at( index.row() ) );
        default:
            return QVariant();
        }
    }
    return QVariant();
}

void BackendModel::refresh()
{
    if ( m_database ) {
        Queries::ReadBackend *query = new Queries::ReadBackend();
        connect( query, &Queries::ReadBackend::readBackend,
                 this, &BackendModel::addBackend, Qt::QueuedConnection );
        m_database->scheduleQuery( query );
    }
}

void BackendModel::addBackend(const QVariant &backend)
{
    Backend *b = new Backend( this );
    b->fromVariant( backend );

    // check if we already have the backend
    for ( Backend *existingBackend : m_backends ) {
        if ( existingBackend->name() == b->name() ) {
            existingBackend->fromVariant( backend );
            delete b;
            return;
        }
    }

    // Still here? Then append:
    beginInsertRows( QModelIndex(), m_backends.size(), m_backends.size() );
    m_backends.append( b );
    endInsertRows();
}


} // namespace Models
} // namespace OpenTodoList
