#include "readtodolist.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

ReadTodoList::ReadTodoList() :
    StorageQuery(),
    m_todoLists(),
    m_currentTodoList( nullptr )
{
}

QList<TodoList *> ReadTodoList::todoLists() const
{
    return m_todoLists;
}

bool ReadTodoList::query(QString &query, QVariantMap &args)
{
    query = "SELECT"
            " todoList.id AS id,"
            " todoList.uuid AS uuid,"
            " account.uuid AS accountUuid,"
            " todoList.name AS name,"
            " todoList.lastModificationTime AS lastModificationTime,"
            " todoListMetaAttributeName.name AS metaAttributeName,"
            " todoListMetaAttribute.value AS metaAttributeValue "
            "FROM"
            " todoList "
            "JOIN"
            " account ON todoList.account = account.id "
            "LEFT OUTER JOIN"
            " todoListMetaAttribute ON todoList.id = todoListMetaAttribute.todoList "
            "JOIN"
            " todoListMetaAttributeName ON todoListMetaAttribute.attributeName = todoListMetaAttributeName.id;";
    return true;
}

void ReadTodoList::recordAvailable(const QVariantMap &record)
{
    if ( m_currentTodoList && record.value( "id" ).toInt() != m_currentTodoList->id() ) {
        emit readTodoList( m_currentTodoList->toVariant() );
        m_todoLists << m_currentTodoList;
        m_currentTodoList = nullptr;
    }
    if ( !m_currentTodoList ) {
        m_currentTodoList = new TodoList( this );
        m_currentTodoList->setId( record.value( "id" ).toInt() );
        m_currentTodoList->setUuid( record.value( "uuid" ).toUuid() );
        m_currentTodoList->setAccountUuid( record.value( "accountUuid" ).toUuid() );
        m_currentTodoList->setName( record.value( "name" ).toString() );
        m_currentTodoList->setLastModificationTime( record.value( "lastModificationTime" ).toDateTime() );
    }
    if ( !record.value( "metaAttributeName").isNull() ) {
        QVariantMap metaAttributes = m_currentTodoList->metaAttributes();
        metaAttributes.insert( record.value( "metaAttributeName" ).toString(),
                               record.value( "metaAttributeValue" ).toString() );
    }
}

void ReadTodoList::endRun()
{
    if ( m_currentTodoList ) {
        emit readTodoList( m_currentTodoList->toVariant() );
        m_todoLists << m_currentTodoList;
    }
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
