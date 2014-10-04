#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "core/opentodolistinterfaces.h"

#include <QObject>

namespace OpenTodoList {
namespace DataModel {

class Account : public QObject, public IAccount
{
    Q_OBJECT
public:
    explicit Account(QObject *parent = 0);

    // IAccount interface
    const QUuid &uuid() const override;
    const QString &name() const override;
    void setName(const QString &name) override;
    bool disposed() const override;
    bool isBackendAccount() const override;
    void setBackendAccount( bool backendAccount ) override;
    QDateTime lastModificationTime() const override;
    void setLastModificationTime(const QDateTime &dateTime) override;

signals:

    void nameChanged();
    void lastModificationTimeChanged();

    void changed();

public slots:

private:

    QUuid       m_uuid;
    QString     m_name;
    bool        m_isPermanentlyDeleted;
    bool        m_isBackendAccount;
    QDateTime   m_lastModificationTime;


    // IAccount interface
public:

};

} /* namespace DataModel */
} /* namespace OpenTodoList */

#endif // ACCOUNT_H
