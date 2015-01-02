#include "datamodel/account.h"

namespace OpenTodoList {
namespace DataModel {


Account::Account(QObject *parent) :
  QObject(parent),
  m_hasId( false ),
  m_id( -1 ),
  m_uuid( QUuid() ),
  m_name( QString() ),
  m_backend( QString() ),
  m_dirty( 0 ),
  m_disposed( false ),
  m_metaAttributes( QVariantMap() ),
  m_loading( false )
{
  connect( this, &Account::nameChanged, this, &Account::emitChanged );
  connect( this, &Account::metaAttributesChanged, this, &Account::emitChanged );
}

bool Account::hasId() const
{
  return m_hasId;
}

int Account::id() const
{
  return m_id;
}

void Account::setId(int id)
{
  m_hasId = true;
  if ( m_id != id ) {
    m_id = id;
    emit idChanged();
  }
}

int Account::dirty() const
{
  return m_dirty;
}

void Account::setDirty(int dirty)
{
  m_dirty = dirty;
}

bool Account::disposed() const
{
  return m_disposed;
}

void Account::setDisposed(bool disposed)
{
  m_disposed = disposed;
}

void Account::setBackend(const QString &backend)
{
  if ( m_backend != backend ) {
    m_backend = backend;
    emit backendChanged();
  }
}

const QUuid &Account::uuid() const
{
  return m_uuid;
}

void Account::setUuid(const QUuid &uuid)
{
  if ( m_uuid != uuid ) {
    m_uuid = uuid;
    emit uuidChanged();
  }
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

QString Account::backend() const
{
  return m_backend;
}

QVariantMap Account::metaAttributes() const
{
  return m_metaAttributes;
}

void Account::setMetaAttributes(const QVariantMap &metaAttributes)
{
  m_metaAttributes = metaAttributes;
  emit metaAttributesChanged();
}

/**
   @brief Saves the Account object to a QVariant

   This returns a QVariant representation of the Account object.

   @sa fromVariant()
 */
QVariant Account::toVariant() const
{
  QVariantMap result;
  result.insert( "backend", m_backend );
  result.insert( "dirty", m_dirty );
  result.insert( "disposed", m_disposed );
  if ( m_hasId ) {
    result.insert( "id", m_id );
  }
  result.insert( "metaAttributes", m_metaAttributes );
  result.insert( "name", m_name );
  result.insert( "uuid", m_uuid );
  return result;
}

/**
   @brief Restores the Account from a QVariant representation
   @sa toVariant()
 */
void Account::fromVariant(const QVariant &account)
{
  m_loading = true;
  QVariantMap map = account.toMap();

  // Special handling for id: Only update if identify changes
  if ( m_uuid != map.value( "uuid", QUuid() ).toUuid() || !m_hasId ) {
    m_hasId = map.contains( "id" );
    if ( m_hasId ) {
      setId( map.value( "id", m_id ).toInt() );
    }
  }

  setBackend( map.value( "backend", m_backend ).toString() );
  m_dirty = map.value( "dirty", m_dirty ).toInt();
  m_disposed = map.value( "disposed", m_disposed ).toBool();
  setMetaAttributes( map.value( "metaAttributes", m_metaAttributes ).toMap() );
  setName( map.value( "name", m_name ).toString() );
  setUuid( map.value( "uuid", m_uuid ).toUuid() );
  m_loading = false;
}

void Account::emitChanged()
{
  if ( !m_loading ) {
    emit changed();
  }
}

} /* namespace DataModel */
} /* namespace OpenTodoList */
