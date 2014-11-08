#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "core/opentodolistinterfaces.h"

#include <QObject>

namespace OpenTodoList {
namespace DataModel {

class Account : public QObject, public IAccount
{
    Q_OBJECT
    Q_PROPERTY(bool hasId READ hasId NOTIFY idChanged)
    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(QUuid uuid READ uuid NOTIFY uuidChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString backend READ backend NOTIFY backendChanged)
    Q_PROPERTY(QDateTime lastModificationTime READ lastModificationTime NOTIFY lastModificationTimeChanged)
    Q_PROPERTY(QVariantMap metaAttributes READ metaAttributes NOTIFY metaAttributesChanged)
public:
    explicit Account(QObject *parent = 0);

    Account *clone( QObject *parent = nullptr ) const;

    bool hasId() const;
    int id() const;
    void setId( int id );
    bool dirty() const;
    bool disposed() const;
    void setBackend( const QString &backend );

    // IAccount interface
    const QUuid &uuid() const override;
    void setUuid(const QUuid &uuid) override;
    const QString &name() const override;
    void setName(const QString &name) override;
    QString backend() const override;
    QDateTime lastModificationTime() const override;
    void setLastModificationTime(const QDateTime &dateTime) override;
    QVariantMap metaAttributes() const override;
    void setMetaAttributes(const QVariantMap &metaAttributes) override;

    QVariant toVariant() const;
    void fromVariant(const QVariant &account );


signals:

    void idChanged();
    void uuidChanged();
    void nameChanged();
    void backendChanged();
    void dirtyChanged();
    void disposedChanged();
    void lastModificationTimeChanged();
    void metaAttributesChanged();

    /**
       @brief The Account object has been changed

       This signal is emitted if one of the properties relevant to the frontend have been
       changed. It is not triggered for properties that are used internally by the database
       system only.
     */
    void changed();

public slots:

private:

    bool        m_hasId;
    int         m_id;
    QUuid       m_uuid;
    QString     m_name;
    QString     m_backend;
    bool        m_dirty;
    bool        m_disposed;
    QDateTime   m_lastModificationTime;
    QVariantMap m_metaAttributes;

};

} /* namespace DataModel */
} /* namespace OpenTodoList */

#endif // ACCOUNT_H
