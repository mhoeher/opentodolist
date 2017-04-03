#ifndef KEYSTORE_H
#define KEYSTORE_H

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
    static const QString ServiceName;

    explicit KeyStore(QObject *parent = 0);
    virtual ~KeyStore();

    void saveCredentials(const QString &key, const QString& value,
                         SaveCredentialsResult *resultReceiver = nullptr);
    void loadCredentials(const QString &key,
                         LoadCredentialsResult *resultReceiver = nullptr);
    void deleteCredentials(const QString &key,
                           DeleteCredentialsResult *resultReceiver = nullptr);
signals:

public slots:

private:
    SimpleCrypt             *m_simpleCrypt;
    QSettings               *m_settings;
};


/**
 * @brief A class used to receive the results of a write credentials operation.
 *
 * This class is passed as a callback object to the KeyStore class. It will receive the
 * results of a save operation to the credential store.
 */
class SaveCredentialsResult : public QObject
{
    Q_OBJECT

public:

    explicit SaveCredentialsResult();
    virtual ~SaveCredentialsResult();
};


/**
 * @brief A class used to receive the results of a read credentials operation.
 *
 * This class is passed as a callback object to the KeyStore class. It will receive the
 * results of a load operation to the credential store.
 */
class LoadCredentialsResult : public QObject
{
    Q_OBJECT

public:

    explicit LoadCredentialsResult();
    virtual ~LoadCredentialsResult();
};


/**
 * @brief A class used to receive the results of a delete credentials operation.
 *
 * This class is passed as a callback object to the KeyStore class. It will receive the
 * results of a delete operation to the credential store.
 */
class DeleteCredentialsResult : public QObject
{
    Q_OBJECT

public:

    explicit DeleteCredentialsResult();
    virtual ~DeleteCredentialsResult();
};

#endif // KEYSTORE_H
