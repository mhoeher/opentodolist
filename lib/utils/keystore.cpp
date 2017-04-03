#include "keystore.h"

#include <QSettings>

#include <qtkeychain/keychain.h>

#include <simplecrypt.h>


const QString KeyStore::ServiceName = "OpenTodoList";


/**
 * @brief Constructor.
 */
KeyStore::KeyStore(QObject *parent) : QObject(parent),
    m_simpleCrypt(new SimpleCrypt(Q_UINT64_C(0x0c14257ce25f252f))),
    m_settings(new QSettings())
{
}

/**
 * @brief Destructor.
 */
KeyStore::~KeyStore()
{
    delete m_settings;
    delete m_simpleCrypt;
}

void KeyStore::saveCredentials(const QString& key, const QString& value,
                               SaveCredentialsResult* resultReceiver)
{
    auto job = new QKeychain::WritePasswordJob(ServiceName, this);
    job->setAutoDelete(true);
    job->setSettings(m_settings);
    job->setKey(key);
    job->setBinaryData(m_simpleCrypt->encryptToByteArray(value));
    connect(job, &QKeychain::WritePasswordJob::finished, [=](QKeychain::Job*) {
        if (resultReceiver) {
            // Todo: propagate infos to receiber
        }
    });
}

void KeyStore::loadCredentials(const QString& key, LoadCredentialsResult* resultReceiver)
{

}

void KeyStore::deleteCredentials(const QString& key, DeleteCredentialsResult* resultReceiver)
{

}

SaveCredentialsResult::SaveCredentialsResult() : QObject()
{

}

SaveCredentialsResult::~SaveCredentialsResult()
{

}

LoadCredentialsResult::LoadCredentialsResult() : QObject()
{

}

LoadCredentialsResult::~LoadCredentialsResult()
{

}

DeleteCredentialsResult::DeleteCredentialsResult() : QObject()
{

}

DeleteCredentialsResult::~DeleteCredentialsResult()
{

}
