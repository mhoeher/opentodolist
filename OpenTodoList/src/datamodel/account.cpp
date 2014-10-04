#include "datamodel/account.h"

namespace OpenTodoList {
namespace DataModel {


Account::Account(QObject *parent) :
    QObject(parent),
    m_uuid( QUuid() ),
    m_name( QString() ),
    m_isPermanentlyDeleted( false ),
    m_isBackendAccount( false ),
    m_lastModificationTime( QDateTime() )
{
    connect( this, SIGNAL(nameChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(lastModificationTimeChanged()), this, SIGNAL(changed()) );
}

const QUuid &Account::uuid() const
{
    return m_uuid;
}

const QString &Account::name() const
{
    return m_name;
}

void Account::setName(const QString &name)
{
    if ( name != m_name ) {
        m_name = name;
        emit nameChanged();
    }
}

bool Account::disposed() const
{
    return m_isPermanentlyDeleted;
}

bool Account::isBackendAccount() const
{
    return m_isBackendAccount;
}

void Account::setBackendAccount(bool backendAccount)
{
    if ( m_isBackendAccount != backendAccount ) {
        m_isBackendAccount = backendAccount;
    }
}

QDateTime Account::lastModificationTime() const
{
    return m_lastModificationTime;
}

void Account::setLastModificationTime(const QDateTime &dateTime)
{
    if ( m_lastModificationTime != dateTime ) {
        m_lastModificationTime = dateTime;
        emit lastModificationTimeChanged();
    }
}

} /* namespace DataModel */
} /* namespace OpenTodoList */
