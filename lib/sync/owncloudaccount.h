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
};

#endif // OWNCLOUDACCOUNT_H
