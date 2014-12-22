#include "readtodo.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

ReadTodo::ReadTodo() :
    StorageQuery(),
    m_todos(),
    m_currentTodo( nullptr ),
    m_todoListUuid( QUuid() ),
    m_minDueDate( QDateTime() ),
    m_maxDueDate( QDateTime() ),
    m_showDone( true )
{
}

bool ReadTodo::query(QString &query, QVariantMap &args)
{
    QTextStream stream( &query );
    stream << "SELECT"
              " todo.id AS id,"
              " todo.uuid AS uuid,"
              " todo.weight AS weight,"
              " todo.done AS done,"
              " todo.priority AS priority,"
              " todo.dueDate AS dueDate,"
              " todo.title AS title,"
              " todo.description AS description,"
              " todoList.uuid as todoListUuid,"
              " todoMetaAttributeName.name AS metaAttributeName,"
              " todoMetaAttribute.value AS metaAttributeValue "
              "FROM "
              " todo "
              "INNER JOIN todoList ON todo.todoList = todoList.id "
              "LEFT OUTER JOIN todoMetaAttribute ON todo.id = todoMetaAttribute.todo "
              "INNER JOIN todoMetaAttributeName ON todoMetaAttribute.attributeName = todoMetaAttributeName.id ";

    // Filtering:
    QStringList conditions;
    if ( !m_todoListUuid.isNull() ) {
        conditions.append( " (todoList.uuid = :filterTodoListUuid) ");
        args.insert( "filterTodoListUuid", m_todoListUuid );
    }
    if ( m_minDueDate.isValid() ) {
        conditions.append( " (todo.dueDate >= :filterMinDueDate) " );
        args.insert( "filterMinDueDate", m_minDueDate );
    }
    if ( m_maxDueDate.isValid() ) {
        conditions.append( " (todo.dueDate <= :filterMaxDueDate) " );
        args.insert( "filterMaxDueDate", m_maxDueDate );
    }
    if ( !m_showDone ) {
        conditions.append( " (todo.done) " );
    }
    if ( !conditions.isEmpty() ) {
        stream << " WHERE " << conditions.join( " AND " );
    }

    stream << ";";
    return true;
}

void ReadTodo::recordAvailable(const QVariantMap &record)
{
    if ( m_currentTodo && m_currentTodo->id() != record.value( "id" ).toInt() ) {
        m_todos << m_currentTodo;
        emit readTodo( m_currentTodo->toVariant() );
        m_currentTodo = nullptr;
    }
    if ( !m_currentTodo ) {
        m_currentTodo = new Todo( this );
        m_currentTodo->setId( record.value( "id" ).toInt() );
        m_currentTodo->setUuid( record.value( "uuid" ).toUuid() );
        m_currentTodo->setWeight( record.value( "weight" ).toDouble() );
        m_currentTodo->setDone( record.value( "done" ).toBool() );
        m_currentTodo->setPriority( record.value( "priority" ).toInt() );
        m_currentTodo->setDueDate( record.value( "dueDate" ).toDateTime() );
        m_currentTodo->setTitle( record.value( "title" ).toString() );
        m_currentTodo->setDescription( record.value( "description" ).toString() );
        m_currentTodo->setTodoListUuid( record.value( "todoListUuid" ).toUuid() );
    }
    if ( record.contains( "metaAttributeName" ) ) {
        QVariantMap attrs = m_currentTodo->metaAttributes();
        attrs.insert( record.value( "metaAttributeName" ).toString(),
                      record.value( "metaAttributeValue" ).toString() );
        m_currentTodo->setMetaAttributes( attrs );
    }
}

void ReadTodo::endRun()
{
    if ( m_currentTodo ) {
        m_todos << m_currentTodo;
        emit readTodo( m_currentTodo->toVariant() );
        m_currentTodo = nullptr;
    }
}

QList<Todo *> ReadTodo::todos() const
{
    return m_todos;
}
QUuid ReadTodo::todoListUuid() const
{
    return m_todoListUuid;
}

void ReadTodo::setTodoListUuid(const QUuid &todoListUuid)
{
    m_todoListUuid = todoListUuid;
}

QDateTime ReadTodo::minDueDate() const
{
    return m_minDueDate;
}

void ReadTodo::setMinDueDate(const QDateTime &minDueDate)
{
    m_minDueDate = minDueDate;
}

QDateTime ReadTodo::maxDueDate() const
{
    return m_maxDueDate;
}

void ReadTodo::setMaxDueDate(const QDateTime &maxDueDate)
{
    m_maxDueDate = maxDueDate;
}
bool ReadTodo::showDone() const
{
    return m_showDone;
}

void ReadTodo::setShowDone(bool showDone)
{
    m_showDone = showDone;
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
