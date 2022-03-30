#include "owncloudaccount.h"

#include <SynqClient/libsynqclient.h>

OwnCloudAccount::OwnCloudAccount(QObject* parent) : WebDAVAccount { parent }
{
    setType(OwnCloud);
}

Synchronizer* OwnCloudAccount::createSynchronizer() const
{
    auto result = WebDAVAccount::createSynchronizer();
    auto webDAVSync = qobject_cast<WebDAVSynchronizer*>(result);
    Q_ASSERT(webDAVSync);
    webDAVSync->setServerType(WebDAVSynchronizer::OwnCloud);
    return result;
}

void OwnCloudAccount::fillServerType(SynqClient::WebDAVServerType& type) const
{
    type = SynqClient::WebDAVServerType::OwnCloud;
}
