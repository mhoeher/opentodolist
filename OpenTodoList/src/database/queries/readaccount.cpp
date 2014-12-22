#include "readaccount.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

/**
   @brief Constructor

   Creates a new query. If @p parentizeAccounts is true, the query takes ownership
   of the created account objects. Otherwise, either use accounts() or connect to readAccount()
   to collect the created accounts.
 */
ReadAccount::ReadAccount() :
    StorageQuery(),
    m_accounts(),
    m_currentAccount( nullptr )
{
}

/**
   @brief The list of accounts read from the database
 */
QList<DataModel::Account *> ReadAccount::accounts() const
{
    return m_accounts;
}

bool ReadAccount::query(QString &query, QVariantMap &args)
{
    Q_UNUSED( args );

    query = "SELECT"
            " account.id AS id,"
            " account.uuid AS uuid,"
            " account.name AS name,"
            " backend.name AS backend,"
            " account.dirty AS dirty,"
            " account.disposed AS disposed,"
            " accountMetaAttributeName.name AS attributeName,"
            " accountMetaAttribute.value AS attributeValue "
            " FROM account JOIN backend ON account.backend = backend.id"
            "              LEFT OUTER JOIN accountMetaAttribute ON account.id = accountMetaAttribute.account"
            "              JOIN accountMetaAttributeName ON accountMetaAttribute.attributeName = accountMetaAttributeName.id;";
    return true;
}

void ReadAccount::recordAvailable(const QVariantMap &record)
{
    if ( !m_currentAccount || m_currentAccount->id() != record.value( "id" ).toInt() ) {
        m_accounts << m_currentAccount;
        emit readAccount( m_currentAccount->toVariant() );

        m_currentAccount = new DataModel::Account( this );
        m_currentAccount->setId( record.value( "id" ).toInt() );
        m_currentAccount->setUuid( record.value( "uuid" ).toUuid() );
        m_currentAccount->setName( record.value( "name" ).toString() );
        m_currentAccount->setBackend( record.value( "backend" ).toString() );
        if ( record.value( "attributeName" ).isValid() ) {
            QVariantMap attributes;
            attributes.insert( record.value( "attributeName" ).toString(),
                               record.value( "attributeValue" ).toString() );
            m_currentAccount->setMetaAttributes( attributes );
        }
    } else {
        QVariantMap attributes = m_currentAccount->metaAttributes();
        attributes.insert( record.value( "attributeName" ).toString(),
                           record.value( "attributeValue" ).toString() );
        m_currentAccount->setMetaAttributes( attributes );
    }
}

void ReadAccount::endRun()
{
    if ( m_currentAccount ) {
        m_accounts << m_currentAccount;
        emit readAccount( m_currentAccount->toVariant() );
    }
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
