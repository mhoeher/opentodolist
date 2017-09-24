#include "keystore.h"

#include <QCoreApplication>
#include <QIODevice>
#include <QJsonDocument>
#include <QSettings>

#ifdef OTL_USE_SYSTEM_QT5KEYCHAIN
#include <qt5keychain/keychain.h>
#else
#include <qtkeychain/keychain.h>
#endif

#include <simplecrypt.h>


Q_LOGGING_CATEGORY(keyStore, "net.rpdev.opentodolist.KeyStore", QtWarningMsg)


const QLatin1String KeyStore::ServiceName = QLatin1String("OpenTodoList");


namespace{


static const quint64 SecureSettingsKey = Q_UINT64_C(0x0c14257ce25f252f);
static QSettings::Format KeyStoreSettingsFormat = QSettings::InvalidFormat;


/**
 * @brief Helper method to write settings in a "secured" way.
 *
 * This is used to write settings to a slightly secured settings file. Basically,
 * a symmetric cipher is applied to not have to store any user secrets in plain text.
 * This is not a real protection against attackers - in fact, we rather rely
 * on nobody is able to access the user's files. However, it is still better
 * to not store any stuff unencrypted (especially on desktop systems, where potentially
 * multiple users have access to the device).
 */
bool securedWriteSettings(QIODevice &device, const QSettings::SettingsMap &map) {
    bool result = false;
    auto doc = QJsonDocument::fromVariant(map);
    auto data = doc.toJson();
    SimpleCrypt simpleCrypt(SecureSettingsKey);
    data = simpleCrypt.encryptToByteArray(data.toHex());
    auto bytesWritten = device.write(data);
    if (bytesWritten == data.length()) {
        result = true;
    }
    return result;
}


/**
 * @brief Read securely written settings.
 *
 * This is the reverse of the securedWriteSettings() functions.
 */
bool securedReadSettings(QIODevice &device, QSettings::SettingsMap &map) {
    bool result = false;
    auto data = device.readAll();
    SimpleCrypt simpleCrypt(SecureSettingsKey);
    data = QByteArray::fromHex(simpleCrypt.decryptToByteArray(data));
    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(data, &error);
    if (error.error == QJsonParseError::NoError) {
        map = doc.toVariant().toMap();
        result = true;
    } else {
        qCWarning(keyStore) << "Failed to read key store, encountered a JSON"
                               "parse error:"
                            << error.errorString();
    }
    return result;
}

}


/**
 * @brief Constructor.
 */
KeyStore::KeyStore(QObject *parent) : QObject(parent),
    m_settings(nullptr)
{
    registerSettingsFormat();
    if (KeyStoreSettingsFormat != QSettings::InvalidFormat) {
        m_settings = new QSettings(
                    KeyStoreSettingsFormat,
                    QSettings::UserScope,
                    QCoreApplication::organizationName(),
                    QCoreApplication::applicationName(),
                    this
                    );
        m_settings->setFallbacksEnabled(false);
    }
}

/**
 * @brief Destructor.
 */
KeyStore::~KeyStore()
{
}

void KeyStore::saveCredentials(const QString& key, const QString &value)
{
    auto job = new QKeychain::WritePasswordJob(ServiceName);
    job->setKey(key);
    job->setTextData(value);
    if (m_settings != nullptr) {
        job->setInsecureFallback(true);
        job->setSettings(m_settings);
    }
    job->setAutoDelete(true);
    connect(job, &QKeychain::WritePasswordJob::finished, [=](QKeychain::Job*) {
        bool success = true;
        if (job->error() != QKeychain::NoError) {
            qCWarning(keyStore) << "Failed to save credentials for" << key
                                << ":" << job->errorString();
            success = false;
        } else {
            qCDebug(keyStore) << "Successfully saved credentials for" << key;
        }
        emit credentialsSaved(key, success);
    });
    job->start();
}

void KeyStore::loadCredentials(const QString& key)
{
    auto job = new QKeychain::ReadPasswordJob(ServiceName);
    job->setKey(key);
    if (m_settings != nullptr) {
        job->setInsecureFallback(true);
        job->setSettings(m_settings);
    }
    job->setAutoDelete(true);
    connect(job, &QKeychain::ReadPasswordJob::finished, [=](QKeychain::Job*) {
        bool success = true;
        if (job->error() != QKeychain::NoError) {
            qCWarning(keyStore) << "Failed to read credentials for" << key
                                << ":" << job->errorString();
            success = false;
        } else {
            qCDebug(keyStore) << "Successfully loaded credentials for" << key;
        }
        emit credentialsLoaded(key, job->textData(), success);
    });
    job->start();
}

void KeyStore::deleteCredentials(const QString& key)
{
    auto job = new QKeychain::DeletePasswordJob(ServiceName);
    job->setKey(key);
    if (m_settings != nullptr) {
        job->setInsecureFallback(true);
        job->setSettings(m_settings);
    }
    job->setAutoDelete(true);
    connect(job, &QKeychain::DeletePasswordJob::finished, [=](QKeychain::Job*) {
        bool success = true;
        if (job->error() != QKeychain::NoError) {
            qCWarning(keyStore) << "Failed to delete credentials for" << key
                                << ":" << job->errorString();
            success = false;
        } else {
            qCDebug(keyStore) << "Successfully deleted credentials for" << key;
        }
        emit credentialsDeleted(key, success);
    });
    job->start();
}

/**
 * @brief Create a settings object as fallback if no key store is found.
 *
 * This is a helper function creating a QSettings object as used as fallback when
 * no platform specific key store can be found. This function is intended for
 * testing purposes only, as outside the key store there should be no need to
 * ever use such a settings object.
 */
QSettings* KeyStore::createSettingsFile(const QString& filename, QObject* parent)
{
    QSettings* result = nullptr;

    if (KeyStoreSettingsFormat == QSettings::InvalidFormat) {
        registerSettingsFormat();
    }
    if (KeyStoreSettingsFormat != QSettings::InvalidFormat) {
        result = new QSettings(filename, KeyStoreSettingsFormat, parent);
    }
    return result;
}

/**
 * @brief Registers the custom settings format.
 */
void KeyStore::registerSettingsFormat()
{
    if (KeyStoreSettingsFormat == QSettings::InvalidFormat) {
        KeyStoreSettingsFormat = QSettings::registerFormat(
                    "otlks",
                    &securedReadSettings,
                    &securedWriteSettings,
                    Qt::CaseSensitive);
    }
}
