#include "nextcloudaccount.h"

NextCloudAccount::NextCloudAccount(QObject* parent) : WebDAVAccount { parent }
{
    setType(NextCloud);
}

Synchronizer* NextCloudAccount::createSynchronizer() const
{
    auto result = WebDAVAccount::createSynchronizer();
    auto webDAVSync = qobject_cast<WebDAVSynchronizer*>(result);
    Q_ASSERT(webDAVSync);
    webDAVSync->setServerType(WebDAVSynchronizer::NextCloud);
    return result;
}
