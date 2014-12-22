#include "inserttodo.h"

#include "QTextStream"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

InsertTodo::InsertTodo(Todo *todo) :
    StorageQuery(),
    m_todo( todo ),
    m_state( InsertTodoState ),
    m_waitForId( false )
{
    Q_ASSERT( m_todo != nullptr );
}

bool InsertTodo::query(QString &query, QVariantMap &args)
{
    m_waitForId = false;
    QTextStream stream( &query );

    switch ( m_state ) {

    case InsertTodoState:
    {
        m_waitForId = true;
        if ( m_todo->hasId() ) {
            stream << "WITH todoInfo ( id ) AS ( VALUES ( :todoId ) ) ";
            args.insert( "todoId", m_todo->id() );
        } else {
            stream  << "WITH todoInfo ( id ) AS ( VALUES ( ( SELECT id FROM todo WHERE uuid = :searchUuid ) ) ) ";
            args.insert( "searchUuid", m_todo->uuid().toString() );
        }
        stream << "INSERT OR REPLACE INTO todo "
               << " ( id, uuid, weight, done, priority, dueDate, title, description, todoList ) "
               << "VALUES ("
               << " (SELECT id FROM todoInfo),"
               << " :uuid,"
               << " :weight,"
               << " :done,"
               << " :priority,"
               << " :dueDate,"
               << " :title,"
               << " :description,"
               << " (SELECT id FROM todoList WHERE uuid = :todoListUuid)"
               << ");";
        args.insert( "uuid", m_todo->uuid().toString() );
        args.insert( "weight", m_todo->weight() );
        args.insert( "done", m_todo->done() );
        args.insert( "priority", m_todo->priority() );
        args.insert( "dueDate", m_todo->dueDate() );
        args.insert( "title", m_todo->title() );
        args.insert( "description", m_todo->description() );
        args.insert( "todoListUuid", m_todo->todoListUuid().toString() );

        m_state = m_todo->metaAttributes().isEmpty() ? RemoveExtraneousMetaAttributesState : InsertMetaAttributeNameState;
        return true;
    }

    case InsertMetaAttributeNameState:
    {
        stream << "INSERT OR IGNORE INTO todoMetaAttributeName ( name ) VALUES ";
        int idx = 0;
        for ( QString key : m_todo->metaAttributes().keys() ) {
            QString name = QString( "name%1" ).arg( idx );
            if ( idx > 0 ) {
                stream << ", ";
            }
            stream << "( :" << name << " )";
            args.insert( name, key );
            ++idx;
        }
        m_state = InsertMetaAttributeValueState;

        return true;
    }

    case InsertMetaAttributeValueState:
    {
        if ( m_todo->hasId() ) {
            stream << "WITH todoInfo ( id ) AS ( VALUES ( :todoId ) ) ";
            args.insert( "todoId", m_todo->id() );
        } else {
            stream << "WITH todoInfo ( id ) AS ( VALUES ( "
                   << " ( SELECT id FROM todo WHERE uuid = :todoUuid ) "
                   << ") ";
            args.insert( "todoUuid", m_todo->uuid().toString() );
        }
        stream << "INSERT OR REPLACE INTO todoMetaAttribute "
               << "("
               << "  todo,"
               << "  attributeName,"
               << "  value"
               << ") "
               << "VALUES ";
        int idx = 0;
        for ( QString key : m_todo->metaAttributes().keys() ) {
            QString attrName = QString( "name%1" ).arg( idx );
            QString attrValue = QString( "value%1" ).arg( idx );
            if ( idx > 0 ) {
                stream << ", ";
            }
            stream << "("
                   << " ( SELECT id FROM todoInfo ), "
                   << " ( SELECT id FROM todoMetaAttributeName WHERE name = :" << attrName << " ), "
                   << " :" << attrValue
                   << ")";
            ++idx;
            args.insert( attrName, key );
            args.insert( attrValue, m_todo->metaAttributes().value( key ).toString() );
        }
        stream << ";";
        m_state = RemoveExtraneousMetaAttributesState;
        return true;
    }

    case RemoveExtraneousMetaAttributesState:
    {
        if ( m_todo->hasId() ) {
            stream << "WITH todoInfo ( id ) AS ( VALUES ( :todoId ) ) ";
            args.insert( "todoId", m_todo->id() );
        } else {
            stream << "WITH todoInfo ( id ) AS ( VALUES ( "
                   << " ( SELECT id FROM todo WHERE uuid = :todoUuid ) "
                   << ") ) ";
            args.insert( "todoUuid", m_todo->uuid().toString() );
        }
        if ( m_todo->metaAttributes().isEmpty() ) {
            stream << "DELETE FROM todoMetaAttribute WHERE todo = ( SELECT id FROM todoInfo );";
        } else {
            stream << "DELETE FROM todoMetaAttribute "
                   << "WHERE "
                   << " todo = ( SELECT id FROM todoInfo ) AND "
                   << " attributeName NOT IN "
                   << "  ( SELECT id FROM todoMetaAttributeName WHERE name IN (";
            int idx = 0;
            for ( QString key : m_todo->metaAttributes().keys() ) {
                QString name = QString( "name%1" ).arg( idx );
                if ( idx > 0 ) {
                    stream << ", ";
                }
                stream << ":" << name;
                args.insert( name, key );
                ++idx;
            }
            stream << ") );";
        }
        m_state = FinishedState;
        return true;
    }

    default:
        break;
    }
    return false;
}

void InsertTodo::newIdAvailable(const QVariant &id)
{
    if ( m_waitForId ) {
        m_todo->setId( id.toInt() );
    }
}

void InsertTodo::endRun()
{
    if ( m_state == FinishedState ) {
        emit todoChanged( m_todo->toVariant() );
    }
}

bool InsertTodo::hasNext() const
{
    return m_state != FinishedState;
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
