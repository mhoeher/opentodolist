#include "inserttask.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

InsertTask::InsertTask(Task *task) :
    StorageQuery(),
    m_task( task ),
    m_state( InsertTaskState ),
    m_waitingForId( false )
{
    Q_ASSERT( m_task != nullptr );
}

bool InsertTask::query(QString &query, QVariantMap &args)
{
    QTextStream stream( &query );

    m_waitingForId = false;
    switch ( m_state ) {

    case InsertTaskState:
    {
        m_waitingForId = true;
        if ( m_task->hasId() ) {
            stream << "WITH taskInfo ( id ) AS ( VALUES ( :taskId ) ) ";
            args.insert( "taskId", m_task->id() );
        } else {
            stream << "WITH taskInfo ( id ) AS ( VALUES ( "
                   << " (SELECT id FROM task WHERE uuid = :searchUuid ) ) ) ";
            args.insert( "searchUuid", m_task->uuid().toString() );
        }
        stream << "INSERT OR REPLACE INTO task "
               << " ( id, uuid, weight, done, title, todo ) "
               << "VALUES ( "
               << " (SELECT id FROM taskInfo ), "
               << " :uuid, "
               << " :weight, "
               << " :done, "
               << " :title, "
               << " ( SELECT id FROM todo WHERE uuid = :todoUuid) "
               << " );";
        args.insert( "uuid", m_task->uuid().toString() );
        args.insert( "weight", m_task->weight() );
        args.insert( "done", m_task->done() );
        args.insert( "title", m_task->title() );
        args.insert( "todoUuid", m_task->todoUuid().toString() );
        m_state = m_task->metaAttributes().isEmpty() ?
                    RemoveExtraMetaValuesState : InsertMetaNamesState;
        return true;
    }

    case InsertMetaNamesState:
    {
        stream << "INSERT OR IGNORE INTO taskMetaAttributeName ( name ) VALUES ";
        int idx = 0;
        for ( QString key : m_task->metaAttributes().keys() ) {
            QString name = QString( "name%1" ).arg( idx );
            if ( idx > 0 ) {
                stream << ", ";
            }
            stream << "( :" << name << " )";
            args.insert( name, key );
            ++idx;
        }
        m_state = InsertMetaValuesState;

        return true;
    }

    case InsertMetaValuesState:
    {
        if ( m_task->hasId() ) {
            stream << "WITH taskInfo ( id ) AS ( VALUES ( :taskId ) ) ";
            args.insert( "taskId", m_task->id() );
        } else {
            stream << "WITH taskInfo ( id ) AS ( VALUES ( "
                   << " ( SELECT id FROM task WHERE uuid = :taskUuid ) "
                   << ") ";
            args.insert( "taskUuid", m_task->uuid().toString() );
        }
        stream << "INSERT OR REPLACE INTO taskMetaAttribute "
               << "("
               << "  task,"
               << "  attributeName,"
               << "  value"
               << ") "
               << "VALUES ";
        int idx = 0;
        for ( QString key : m_task->metaAttributes().keys() ) {
            QString attrName = QString( "name%1" ).arg( idx );
            QString attrValue = QString( "value%1" ).arg( idx );
            if ( idx > 0 ) {
                stream << ", ";
            }
            stream << "("
                   << " ( SELECT id FROM taskInfo ), "
                   << " ( SELECT id FROM taskMetaAttributeName WHERE name = :" << attrName << " ), "
                   << " :" << attrValue
                   << ")";
            ++idx;
            args.insert( attrName, key );
            args.insert( attrValue, m_task->metaAttributes().value( key ).toString() );
        }
        stream << ";";
        m_state = RemoveExtraMetaValuesState;
        return true;
    }

    case RemoveExtraMetaValuesState:
    {
        if ( m_task->hasId() ) {
            stream << "WITH taskInfo ( id ) AS ( VALUES ( :taskId ) ) ";
            args.insert( "taskId", m_task->id() );
        } else {
            stream << "WITH taskInfo ( id ) AS ( VALUES ( "
                   << " ( SELECT id FROM task WHERE uuid = :todoUuid ) "
                   << ") ) ";
            args.insert( "taskUuid", m_task->uuid().toString() );
        }
        if ( m_task->metaAttributes().isEmpty() ) {
            stream << "DELETE FROM taskMetaAttribute WHERE task = ( SELECT id FROM taskInfo );";
        } else {
            stream << "DELETE FROM taskMetaAttribute "
                   << "WHERE "
                   << " task = ( SELECT id FROM taskInfo ) AND "
                   << " attributeName NOT IN "
                   << "  ( SELECT id FROM taskMetaAttributeName WHERE name IN (";
            int idx = 0;
            for ( QString key : m_task->metaAttributes().keys() ) {
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
        return false;

    }
}

void InsertTask::newIdAvailable(const QVariant &id)
{
    if ( m_waitingForId ) {
        m_task->setId( id.toInt() );
    }
}

void InsertTask::endRun()
{
    if ( m_state == FinishedState ) {
        emit taskChanged( m_task->toVariant() );
    }
}

bool InsertTask::hasNext() const
{
    return m_state != FinishedState;
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
