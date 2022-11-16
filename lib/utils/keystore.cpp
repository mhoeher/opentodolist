/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "keystore.h"

#include <QCoreApplication>
#include <QIODevice>
#include <QJsonDocument>
#include <QSettings>
#include <QTimer>
#include <QPointer>

#ifdef Q_OS_ANDROID
#    include <QAndroidJniExceptionCleaner>
#    include <QAndroidJniObject>
#    include <QtAndroid>
#else
#    if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#        include <qt5keychain/keychain.h>
#    else
#        include <qt6keychain/keychain.h>
#    endif
#endif

#include <simplecrypt.h>

static Q_LOGGING_CATEGORY(log, "OpenTodoList.KeyStore", QtDebugMsg);

const QLatin1String KeyStore::ServiceName = QLatin1String("OpenTodoList");

namespace {

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
bool securedWriteSettings(QIODevice& device, const QSettings::SettingsMap& map)
{
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
bool securedReadSettings(QIODevice& device, QSettings::SettingsMap& map)
{
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
        qCWarning(log) << "Failed to read key store, encountered a JSON"
                          "parse error:"
                       << error.errorString();
    }
    return result;
}

#ifdef Q_OS_ANDROID
QByteArray fromArray(const jbyteArray array)
{
    QAndroidJniEnvironment env;
    jbyte* const bytes = env->GetByteArrayElements(array, nullptr);
    const QByteArray result(reinterpret_cast<const char*>(bytes), env->GetArrayLength(array));
    env->ReleaseByteArrayElements(array, bytes, JNI_ABORT);
    return result;
}

QAndroidJniObject toArray(const QByteArray& bytes)
{
    QAndroidJniEnvironment env;
    const int length = bytes.length();
    QAndroidJniObject array = QAndroidJniObject::fromLocalRef(env->NewByteArray(length));
    env->SetByteArrayRegion(static_cast<jbyteArray>(array.object()), 0, length,
                            reinterpret_cast<const jbyte*>(bytes.constData()));
    return array;
}
#endif // Q_OS_ANDROID

} // namespace

/**
 * @brief Constructor.
 */
KeyStore::KeyStore(QObject* parent) : QObject(parent), m_settings(nullptr)
{
    registerSettingsFormat();
    if (KeyStoreSettingsFormat != QSettings::InvalidFormat) {
        m_settings = new QSettings(KeyStoreSettingsFormat, QSettings::UserScope,
                                   QCoreApplication::organizationName(),
                                   QCoreApplication::applicationName(), this);
        m_settings->setFallbacksEnabled(false);
    }
}

/**
 * @brief Destructor.
 */
KeyStore::~KeyStore() {}

/**
 * @brief Store a secret.
 *
 * This stores the secret @p value using the @p key in the secure storage.
 * By default, if we cannot access the secure storage, we save the secret to a
 * fallback location. This fallback is only weakly encrypted and rather relies
 * on the environment we run in to protect us (like access rights or a (full)
 * disk encryption.
 *
 * If @p removeCopyFromInsecureFallbackOnSuccess is true, we won't try to
 * write the secret to the fallback. Instead, we check if the secret has
 * previously been stored in the fallback location and - if so - remove it from
 * there in case the save operation succeeded. This can be used to transfer
 * secrets to the secret store after it previously has been stored in the
 * insecure fallback.
 *
 * @note On Android, this saves the secrets encrypted (with a key protected by Android's KeyStore)
 * to the default config file into the section `KeyStore`.
 */
void KeyStore::saveCredentials(const QString& key, const QString& value,
                               bool removeCopyFromInsecureFallbackOnSuccess)
{
#ifdef Q_OS_ANDROID
    Q_UNUSED(removeCopyFromInsecureFallbackOnSuccess);

    // On Android, QtKeychain provides no means to securely safe data out of the box on their latest
    // master. Hence, we store ourselves, using Android's key store to create and securely store a
    // key for encrypting the password:
    bool success = false;
    QSettings settings;
    settings.beginGroup("KeyStore");
    QAndroidJniExceptionCleaner cleaner(QAndroidJniExceptionCleaner::OutputMode::Verbose);
    auto jContext = QtAndroid::androidContext();
    auto jService = QAndroidJniObject::fromString(ServiceName);
    auto jKey = QAndroidJniObject::fromString(key);
    auto jPlainText = toArray(value.toUtf8());
    auto ret = QAndroidJniObject::callStaticObjectMethod(
            "net/rpdev/OpenTodoList/CryptoUtils", "encrypt",
            "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;[B)Lnet/rpdev/"
            "OpenTodoList/CryptoUtils$EncryptionResult;",
            jContext.object<jobject>(), jService.object<jstring>(), jKey.object<jstring>(),
            jPlainText.object<jbyteArray>());
    if (ret.isValid()) {
        auto jSuccess = ret.getField<jboolean>("success");
        if (jSuccess) {
            auto jCipherText = ret.getObjectField<jbyteArray>("cipherText");
            auto cipherText = fromArray(jCipherText.object<jbyteArray>());
            if (!cipherText.isEmpty()) {
                success = true;
                // Store in <key>/data, this is compatible with what the QtKeychain version we
                // used before is doing - so users with existing passwords shouldn't notice a
                // difference.
                settings.setValue(key + "/data", cipherText.toHex());
                settings.sync();
            }
        }
    }

    if (!success && m_settings) {
        // Saving failed, use (insecure) fallback. As long as the device is not
        // rooted this should be save enough. Unfortunately, this is required to
        // work around
        // https://stackoverflow.com/questions/44469020/androidkeystore-keypairgenerator-crashes-on-small-number-of-devices
        m_settings->beginGroup("Fallback");
        m_settings->setValue(key, value);
        m_settings->endGroup();
        success = true;
    }

    // To be compatible with QtKeychain, emit saved signal delayed
    QPointer<KeyStore> _this = this;
    QTimer::singleShot(0, [=]() {
        if (_this) {
            emit _this.data()->credentialsSaved(key, success);
        }
    });
#else
    qCDebug(log) << "Saving secret for" << key;
    auto job = new QKeychain::WritePasswordJob(ServiceName);
    job->setKey(key);
    job->setTextData(value);
    if (m_settings != nullptr) {
        job->setInsecureFallback(true);
        job->setSettings(m_settings);
    }
    job->setAutoDelete(true);
    connect(job, &QKeychain::WritePasswordJob::finished, this, [=](QKeychain::Job*) {
        qCDebug(log) << "Writing secret for" << key << "finished" << job->error()
                     << job->errorString();
        bool success = true;
        if (job->error() != QKeychain::NoError) {
            if (removeCopyFromInsecureFallbackOnSuccess) {
                qCWarning(log) << "Failed to write secret for" << key
                               << "to the secret store - giving up";
            } else {
                qCWarning(log) << "Failed to save credentials for" << key << ":"
                               << job->errorString() << "- using fallback";
                m_settings->beginGroup("Fallback");
                m_settings->setValue(key, value);
                m_settings->endGroup();
            }
        } else {
            qCDebug(log) << "Successfully saved credentials for" << key;
            if (removeCopyFromInsecureFallbackOnSuccess) {
                m_settings->beginGroup("Fallback");
                if (m_settings->contains(key)) {
                    m_settings->remove(key);
                    qCWarning(log) << "Removed secret for" << key << "from the insecure fallback";
                }
                m_settings->endGroup();
            }
        }
        emit credentialsSaved(key, success);
    });
    job->start();
#endif
}

/**
 * @brief Load previously saved credentials.
 *
 * This tries to load the credentials associated with the given @p key and emit the
 * credentialsLoaded() signal once the operation finishes.
 *
 * @note On Android, we try to read the key both from the insecure fallback as well as the default
 * config file for the app. This is because previously we used to rely on a non-official extension
 * of QtKeychain to handle secret storage on Android (which was basically exactly this). Later
 * versions do the encryption of the secrets themselves and directly store the secrets in the
 * default config file. This was also required in order to prevent a change/bug, where after a
 * restart of the app the data would not be correctly be read back from the insecure fallback.
 * @param key
 */
void KeyStore::loadCredentials(const QString& key)
{
#ifdef Q_OS_ANDROID
    // Reverse the encryption:
    bool success = false;
    QString value;

    QSettings settings;
    settings.beginGroup("KeyStore");
    for (auto s : { m_settings, &settings }) {
        if (s && s->contains(key + "/data")) {
            QAndroidJniExceptionCleaner cleaner(QAndroidJniExceptionCleaner::OutputMode::Verbose);
            auto jService = QAndroidJniObject::fromString(ServiceName);
            auto jKey = QAndroidJniObject::fromString(key);
            auto cipherText = QByteArray::fromHex(s->value(key + "/data").toByteArray());
            auto jCipherText = toArray(cipherText);
            auto ret = QAndroidJniObject::callStaticObjectMethod(
                    "net/rpdev/OpenTodoList/CryptoUtils", "decrypt",
                    "(Ljava/lang/String;Ljava/lang/String;[B)Lnet/rpdev/OpenTodoList/CryptoUtils$"
                    "DecryptionResult;",
                    jService.object<jstring>(), jKey.object<jstring>(),
                    jCipherText.object<jbyteArray>());
            if (ret.isValid()) {
                auto jSuccess = ret.getField<jboolean>("success");
                if (jSuccess) {
                    auto jPlainText = ret.getObjectField<jbyteArray>("plainText");
                    auto plainText = fromArray(jPlainText.object<jbyteArray>());
                    if (!plainText.isEmpty()) {
                        success = true;
                        value = QString::fromUtf8(plainText);
                        break;
                    }
                }
            }
        }
    }

    if (!success && m_settings) {
        m_settings->beginGroup("Fallback");
        if (m_settings->contains(key)) {
            value = m_settings->value(key).toString();
            success = true;
        }
        m_settings->endGroup();
    }

    // To be compatible with QtKeychain, emit saved signal delayed
    QPointer<KeyStore> _this = this;
    QTimer::singleShot(0, [=]() {
        if (_this) {
            emit _this.data()->credentialsLoaded(key, value, success);
        }
    });
#else
    qCDebug(log) << "Reading secrets for" << key;
    auto job = new QKeychain::ReadPasswordJob(ServiceName);
    job->setKey(key);
    if (m_settings != nullptr) {
        job->setInsecureFallback(true);
        job->setSettings(m_settings);
    }
    job->setAutoDelete(true);
    connect(job, &QKeychain::ReadPasswordJob::finished, this, [=](QKeychain::Job*) {
        qCDebug(log) << "Reading secrets for" << key << "finished:" << job->error()
                     << job->errorString();
        bool success = true;
        QString secret = job->textData();
        if (job->error() != QKeychain::NoError) {
            qCWarning(log) << "Failed to read credentials for" << key << ":" << job->errorString()
                           << "- trying fallback";
            m_settings->beginGroup("Fallback");
            if (m_settings->contains(key)) {
                secret = m_settings->value(key).toString();

                // We previously were unable to store the secret in the
                // secret service and hence used the (insecure) fallback.
                // Let's try again to write the secret to the secure storage
                // and (if that succeeds) remove the secret from the fallback
                // location:
                saveCredentials(key, secret, true);
            } else {
                success = false;
            }
            m_settings->endGroup();
        } else {
            qCDebug(log) << "Successfully loaded credentials for" << key;
        }
        emit credentialsLoaded(key, secret, success);
    });
    QTimer::singleShot(3000, job, &QKeychain::Job::start);
#endif
}

void KeyStore::deleteCredentials(const QString& key)
{
#ifdef Q_OS_ANDROID
    // Remove the encryption key for the specified key:
    bool success = false;

    {
        QAndroidJniExceptionCleaner cleaner(QAndroidJniExceptionCleaner::OutputMode::Verbose);
        auto jService = QAndroidJniObject::fromString(ServiceName);
        auto jKey = QAndroidJniObject::fromString(key);
        auto ret = QAndroidJniObject::callStaticMethod<jboolean>(
                "net/rpdev/OpenTodoList/CryptoUtils", "deleteKey",
                "(Ljava/lang/String;Ljava/lang/String;)Z", jService.object<jstring>(),
                jKey.object<jstring>());
        success = ret;
        m_settings->remove(key + "/data");
        m_settings->sync();
    }

    if (m_settings) {
        m_settings->beginGroup("Fallback");
        m_settings->remove(key);
        m_settings->endGroup();
    }

    // To be compatible with QtKeychain, emit saved signal delayed
    QPointer<KeyStore> _this = this;
    QTimer::singleShot(0, [=]() {
        if (_this) {
            emit _this.data()->credentialsDeleted(key, success);
        }
    });
#else
    // Remove from fallback (in case we stored twice):
    m_settings->beginGroup("Fallback");
    m_settings->remove(key);
    m_settings->endGroup();

    // Try to remove from platform specific key store:
    auto job = new QKeychain::DeletePasswordJob(ServiceName);
    job->setKey(key);
    if (m_settings != nullptr) {
        job->setInsecureFallback(true);
        job->setSettings(m_settings);
    }
    job->setAutoDelete(true);
    connect(job, &QKeychain::DeletePasswordJob::finished, this, [=](QKeychain::Job*) {
        bool success = true;
        if (job->error() != QKeychain::NoError) {
            qCWarning(log) << "Failed to delete credentials for" << key << ":"
                           << job->errorString();
            success = false;
        } else {
            qCDebug(log) << "Successfully deleted credentials for" << key;
        }
        emit credentialsDeleted(key, success);
    });
    job->start();
#endif
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
                "otlks", &securedReadSettings, &securedWriteSettings, Qt::CaseSensitive);
    }
}
