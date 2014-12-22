#include "insertaccount.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

/**
   @brief Constructor

   Creates a new InsertAccount object, which will upon run insert or update the
   @p account in the Database. The insertion is influenced by the @p flags. See
   documentation for InsertFlag.

   @note By default, the query takes over ownership of the account. Pass
         StorageQuery::CallerIsOwner as @p ownership to keep ownership untouched.
 */
InsertAccount::InsertAccount(DataModel::Account *account, InsertFlags flags, ResourceOwnership ownership) :
    OpenTodoList::DataBase::StorageQuery(),
    m_account( account ),
    m_state( InsertAccountState ),
    m_flags( flags ),
    m_ownership( ownership ),
    m_waitForId( true )
{
    Q_ASSERT( account != nullptr );
}

/**
   @brief Destructor
 */
InsertAccount::~InsertAccount()
{
    if ( m_ownership == QueryIsOwner ) {
        delete m_account;
    }
}

bool InsertAccount::query(QString &query, QVariantMap &args)
{
    m_waitForId = false;
    switch ( m_state ) {

    case InsertAccountState:
    {
        m_waitForId = true;
        query = "WITH "
                "  accInfo ( id ) AS ( VALUES ( %1 ) ), "
                "  backendInfo ( id ) as ( VALUES ( ( SELECT id FROM backend WHERE name = :backend ) ) ) "
                "INSERT OR REPLACE INTO account ( %2 ) "
                "VALUES ( %3 );";

        if ( m_account->hasId() ) {
            query = query.arg( m_account->id() );
        } else {
            query = query.arg( " ( SELECT id FROM account WHERE uuid = :searchUuid ) " );
            args.insert( "searchUuid", m_account->uuid().toString() );
        }

        args.insert( "backend", m_account->backend() );


        QStringList names;
        QStringList values;

        // Insert id
        names << "id";
        values << "( SELECT id FROM accInfo )";

        // insert uuid
        names << "uuid";
        values << ":uuid";
        args.insert( "uuid", m_account->uuid().toString() );

        // insert name
        names << "name";
        values << ":name";
        args.insert( "name", m_account->name() );

        // insert backend
        names << "backend";
        values << "( SELECT id FROM backendInfo )";

        // insert dirty flag
        if ( m_flags.testFlag( MarkAsDirty ) || m_flags.testFlag( MarkAsClean ) ) {
            names << "dirty";
            values << ":dirty";
            args.insert( "dirty", m_flags.testFlag( MarkAsDirty ) );
        }

        // insert disposed flags
        if ( m_flags.testFlag( MarkAsDisposed ) ) {
            names << "disposed";
            values << ":disposed";
            args.insert( "disposed", true );
        }

        query = query.arg( names.join( ", " ) ).arg( values.join( ", " ) );

        m_state = m_account->metaAttributes().isEmpty() ? RemoveExtraMetaState : InsertMetaNameState;
        return true;
    }

    case InsertMetaNameState:
    {
        query = "INSERT OR IGNORE INTO accountMetaAttributeName "
                " ( name ) "
                "VALUES %1;";
        QStringList names;
        int idx = 1;
        for ( const QString &name : m_account->metaAttributes().keys() ) {
            names << QString( "( :name%1 )" ).arg( idx );
            args.insert( QString( "name%1" ).arg( idx ), name );
            ++idx;
        }
        query = query.arg( names.join( ", " ) );
        m_state = InsertMetaState;
        return true;
    }

    case InsertMetaState:
    {
        query = "WITH accInfo ( id ) AS ( VALUES ( %1 ) ) "
                "INSERT OR REPLACE INTO accountMetaAttribute "
                " ( account, attributeName, value ) "
                "VALUES %2;";

        if ( m_account->hasId() ) {
            query = query.arg( ":accountId" );
            args.insert( "accountId", m_account->id() );
        } else {
            query = query.arg( " ( SELECT id FROM account WHERE uuid = :accountUuid ) " );
            args.insert( "accountUuid", m_account->uuid().toString() );
        }

        QStringList values;
        int idx = 1;
        for ( const QString &name : m_account->metaAttributes().keys() ) {
            values << QString(
                          "( ( SELECT id FROM accInfo ), "
                          "  ( SELECT id FROM accountMetaAttributeName WHERE name = :name%1 ), "
                          "  :value%1 )" ).arg( idx );
            args.insert( QString( "name%1" ).arg( idx ), name );
            args.insert( QString ( "value%1" ).arg( idx ),
                         m_account->metaAttributes().value( name ).toString() );
            ++idx;
        }
        query = query.arg( values.join( ", " ) );

        m_state = RemoveExtraMetaState;
        return true;
    }

    case RemoveExtraMetaState:
    {
        query = "WITH accInfo ( id ) AS ( VALUES ( %1 ) ) "
                "DELETE FROM accountMetaAttribute WHERE ( account = ( SELECT id FROM accInfo ) %2 );";
        if ( m_account->hasId() ) {
            query = query.arg( ":accountId" );
            args.insert( "accountId", m_account->id() );
        } else {
            query = query.arg( " ( SELECT id FROM account WHERE uuid = :accountUuid ) " );
            args.insert( "accountUuid", m_account->uuid().toString() );
        }

        if ( m_account->metaAttributes().isEmpty() ) {
            query = query.arg( "" );
        } else {
            QStringList attrNames = m_account->metaAttributes().keys();
            QStringList attrNamePlaceholders;
            for ( int i = 0; i < attrNames.size(); ++i ) {
                QString attrNamePlaceholder = QString( "attrName%1" ).arg( i + 1 );
                attrNamePlaceholders << ( ":" + attrNamePlaceholder );
                args.insert( attrNamePlaceholder, attrNames.at( i ) );
            }
            query = query.arg( QString (
                                   "AND attributeName NOT IN "
                                   "( SELECT id FROM accountMetaAttributeName WHERE name IN ( %1 ) ) " )
                               .arg( attrNamePlaceholders.join( ", " ) ) );
        }

        m_state = DoneState;
        return true;
    }

    default:
        return false;
    }
}

bool InsertAccount::hasNext() const
{
    return m_state != DoneState;
}

void InsertAccount::newIdAvailable(const QVariant &id)
{
    if ( m_waitForId ) {
        if ( id.isValid() ) {
            m_account->setId( id.toInt() );
        }
    }
}

void InsertAccount::endRun()
{
    if ( m_state = DoneState ) {
        emit accountChanged( m_account->toVariant() );
    }
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
