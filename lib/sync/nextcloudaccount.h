#ifndef NEXTCLOUDACCOUNT_H
#define NEXTCLOUDACCOUNT_H

#include <QObject>
#include "webdavaccount.h"

class NextCloudAccount : public WebDAVAccount
{
    Q_OBJECT
public:
    explicit NextCloudAccount(QObject* parent = nullptr);

signals:

    // Account interface
public:
    Synchronizer* createSynchronizer() const override;
};

#endif // NEXTCLOUDACCOUNT_H
