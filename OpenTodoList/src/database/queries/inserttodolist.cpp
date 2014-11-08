#include "inserttodolist.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

InsertTodoList::InsertTodoList( TodoList *todoList ) :
    StorageQuery(),
    m_state( InsertTodoListState ),
    m_todoList( todoList ),
    m_waitForId( false )
{
    Q_ASSERT( m_todoList != nullptr );
}

bool InsertTodoList::query(QString &query, QVariantMap &args)
{
    m_waitForId = false;

    switch ( m_state ) {

    case InsertTodoListState:
    {
        m_waitForId = true;
        query = "INSERT OR REPLACE INTO todoList "
                " ( id, uuid, account, name, lastModificationTime ) "
                " VALUES ( "
                "  ( SELECT id FROM todoList WHERE uuid = :searchUuid ),"
                "  :uuid, "
                "  ( SELECT id FROM account WHERE uuid = :searchAccountUuid ),"
                "  :name,"
                "  :lastModificationTime"
                " );";
        args.insert( "searchUuid", m_todoList->uuid().toString() );
        args.insert( "uuid", m_todoList->uuid().toString() );
        args.insert( "searchAccountUuid", m_todoList->accountUuid().toString() );
        args.insert( "name", m_todoList->name() );
        args.insert( "lastModificationTime", m_todoList->lastModificationTime() );

        m_state = m_todoList->metaAttributes().isEmpty() ?
                    RemoveExtraneousMetaAttributeValuesState :
                    InsertMetaAttributeNamesState;
        return true;
    }

    case InsertMetaAttributeNamesState:
    {
        query = "INSERT OR IGNORE INTO todoListMetaAttributeName "
                " ( name ) "
                " VALUES %1";
        QStringList values;
        int idx = 0;
        for ( QString key : m_todoList->metaAttributes().keys() ) {
            values << QString(
                          "( :name%1 )"
                          ).arg( idx );
            args.insert( QString( "name%1" ).arg( idx ), key );
            ++idx;
        }
        query = query.arg( values.join( ", " ) );
        m_state = InsertMetaAttributeValuesState;
        return true;
    }

    case InsertMetaAttributeValuesState:
    {
        query = "INSERT OR REPLACE INTO todoListMetaAttribute "
                " ( todoList, attributeName, value ) "
                " VALUES %1;";
        QStringList values;
        int idx = 0;
        for ( QString key : m_todoList->metaAttributes().keys() ) {
            QString value = m_todoList->metaAttributes().value( key ).toString();
            values << QString(
              "( (SELECT id FROM todoList WHERE uuid = :todoListUuid%1),"
              "  (SELECT id FROM todoListMetaAttributeName WHERE name = :attributeName%1),"
              "  :value%1 )" ).arg( idx );
            args.insert( QString("todoListUuid%1").arg(idx), m_todoList->uuid() );
            args.insert( QString("attributeName%1").arg( idx ), key );
            args.insert( QString( "value%1" ).arg(idx), value );
            ++idx;
        }
        query = query.arg( values.join( ", " ) );
        m_state = RemoveExtraneousMetaAttributeValuesState;
        return true;
    }

    case RemoveExtraneousMetaAttributeValuesState:
    {
        if ( m_todoList->metaAttributes().isEmpty() ) {
            query = "DELETE FROM todoListMetaAttribute "
                    "WHERE todoList = "
                    " ( SELECT id FROM todoList WHERE uuid = :searchUuid ) );";
            args.insert( "searchUuid", m_todoList->uuid() );
        } else {
            query = "DELETE FROM todoListMetaAttribute "
                    "WHERE todoList = "
                    " ( SELECT id FROM todoList WHERE uuid = :searchUuid ) AND"
                    " attributeName NOT IN"
                    " ( SELECT id FROM todoListMetaAttributeName WHERE name IN ( %1 ) );";
            QStringList attributeNames;
            int idx = 0;
            for ( QString key : m_todoList->metaAttributes().keys() ) {
                attributeNames.append( QString( ":attrName%1" ).arg( idx ) );
                args.insert( QString( "attrName%1" ).arg( idx ), key );
                ++idx;
            }
            query = query.arg( attributeNames.join( ", " ) );
        }
        m_state = FinishedState;
        return true;
    }

    default:
        return false;

    }
}

void InsertTodoList::newIdAvailable(const QVariant &id)
{
    if ( m_waitForId ) {
        m_todoList->setId( id.toInt() );
    }
}

void InsertTodoList::endRun()
{
    if ( m_state == FinishedState ) {
        emit todoListChanged( m_todoList->toVariant() );
    }
}

bool InsertTodoList::hasNext() const
{
    return m_state != FinishedState;
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
