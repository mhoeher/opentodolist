#ifndef OWNCLOUDACCOUNT_H
#define OWNCLOUDACCOUNT_H

#include <QObject>

#include "webdavaccount.h"

class OwnCloudAccount : public WebDAVAccount
{
    Q_OBJECT
public:
    explicit OwnCloudAccount(QObject* parent = nullptr);

signals:

    // Account interface
public:
    Synchronizer* createSynchronizer() const override;

    // WebDAVAccount interface
protected:
    void fillServerType(SynqClient::WebDAVServerType& type) const override;
};

#endif // OWNCLOUDACCOUNT_H
