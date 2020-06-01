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

#ifndef UTILS_KEYSTORE_H_
#define UTILS_KEYSTORE_H_

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

    explicit KeyStore(QObject* parent = 0);
    virtual ~KeyStore();

    void saveCredentials(const QString& key, const QString& value,
                         bool removeCopyFromInsecureFallbackOnSuccess = false);
    void loadCredentials(const QString& key);
    void deleteCredentials(const QString& key);

    static QSettings* createSettingsFile(const QString& filename, QObject* parent = nullptr);

signals:

    /**
     * @brief The credentials for the given @p key have been saved.
     */
    void credentialsSaved(const QString& key, bool success);

    /**
     * @brief The credentials for the given @p key have been loaded.
     *
     * The @p value contains the loaded secret. The @p success flag indicates
     * if the secret was loaded successfully.
     */
    void credentialsLoaded(const QString& key, const QString& value, bool success);

    /**
     * @brief Credentials for the @p key have been removed.
     */
    void credentialsDeleted(const QString& key, bool success);

public slots:

private:
    QSettings* m_settings;

    static void registerSettingsFormat();
};

#endif // UTILS_KEYSTORE_H_
