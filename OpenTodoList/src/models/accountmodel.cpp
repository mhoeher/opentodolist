#include "accountmodel.h"

namespace OpenTodoList {
namespace Models {

AccountModel::AccountModel(QObject *parent) :
    QAbstractListModel(parent)
{
}
Database *AccountModel::database() const
{
    return m_database;
}

void AccountModel::setDatabase(Database *database)
{
    if ( m_database != database ) {
        if ( m_database ) {
            disconnect( m_database, &Database::accountChanged,
                        this, &AccountModel::addAccount );
        }
        m_database = database;
        if ( m_database ) {
            connect( m_database, &Database::accountChanged,
                     this, &AccountModel::addAccount );
        }
        emit databaseChanged();
    }
}

int AccountModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.isValid() ) {
        return 0;
    }
    return m_accounts.size();
}

QVariant AccountModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() && index.row() < m_accounts.size() ) {
        switch ( role ) {
        case Qt::DisplayRole: return QVariant::fromValue< QObject* >( m_accounts.at( index.row() ) );
        default:
            break;
        }
    }
    return QVariant();
}

void AccountModel::refresh()
{
    if ( m_database ) {
        Queries::ReadAccount *query = new Queries::ReadAccount();
        connect( query, &Queries::ReadAccount::readAccount,
                 this, &AccountModel::addAccount, Qt::QueuedConnection );
        m_database->scheduleQuery( query );
    }
}

void AccountModel::addAccount(const QVariant &account)
{
    Account *a = new Account( this );
    a->fromVariant( account );

    // check if the already have the account and update it
    for ( Account *existingAccount : m_accounts ) {
        if ( existingAccount->uuid() == a->uuid() ) {
            existingAccount->fromVariant( account );
            delete a;
            return;
        }
    }

    // append account to end of list
    beginInsertRows( QModelIndex(), m_accounts.size(), m_accounts.size() );
    m_accounts.append( a );
    endInsertRows();
}


} // namespace Models
} // namespace OpenTodoList
