#include "nextcloudaccount.h"

#include <SynqClient/libsynqclient.h>

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

void NextCloudAccount::fillServerType(SynqClient::WebDAVServerType& type) const
{
    type = SynqClient::WebDAVServerType::NextCloud;
}
