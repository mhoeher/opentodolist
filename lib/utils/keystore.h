#ifndef KEYSTORE_H
#define KEYSTORE_H

#include <QLoggingCategory>
#include <QObject>
#include <QSet>

class DeleteCredentialsResult;
class LoadCredentialsResult;
class QSettings;
class SaveCredentialsResult;
class SimpleCrypt;

/**
 * @brief Provides means for persisting user secrets.
 *
 * The KeyStore class can be used to store user secrets like passwords.
 * Depending on the OS and installed frameworks, it tries to store this
 * information in a secure way. In case such a backend is not available,
 * it will write the information in an obfuscated way.
 */
class KeyStore : public QObject
{
    Q_OBJECT
public:
    static const QLatin1String ServiceName;

    explicit KeyStore(QObject *parent = 0);
    virtual ~KeyStore();

    void saveCredentials(const QString &key, const QString &value,
                         bool removeCopyFromInsecureFallbackOnSuccess = false);
    void loadCredentials(const QString &key);
    void deleteCredentials(const QString &key);

    static QSettings *createSettingsFile(const QString &filename, QObject *parent = nullptr);

signals:

    /**
     * @brief The credentials for the given @p key have been saved.
     */
    void credentialsSaved(const QString &key, bool success);

    /**
     * @brief The credentials for the given @p key have been loaded.
     *
     * The @p value contains the loaded secret. The @p success flag indicates
     * if the secret was loaded successfully.
     */
    void credentialsLoaded(const QString &key, const QString &value, bool success);

    /**
     * @brief Credentials for the @p key have been removed.
     */
    void credentialsDeleted(const QString &key, bool success);

public slots:

private:
    QSettings *m_settings;

    static void registerSettingsFormat();
};

#endif // KEYSTORE_H
