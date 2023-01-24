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

#ifndef DATASTORAGE_APPLICATIONSETTINGS_H_
#define DATASTORAGE_APPLICATIONSETTINGS_H_

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QPointer>
#include <QVariantMap>

class Library;
class QSettings;
class Cache;
class Account;
class KeyStore;
class ProblemManager;

class ApplicationSettings : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationSettings(Cache* cache, KeyStore* keyStore, ProblemManager* problemManager,
                                 QObject* parent = nullptr);
    explicit ApplicationSettings(const QString& applicationDir, Cache* cache, KeyStore* keyStore,
                                 ProblemManager* problemManager, QObject* parent = nullptr);
    ~ApplicationSettings() override;

    QSettings& settings() const;
    QList<QSharedPointer<Library>> librariesFromConfig();
    void librariesToConfig(QList<QSharedPointer<Library>> libraries);
    void saveAccount(Account* account);
    void loadSecretsForAccount(Account* account);

    QSharedPointer<Library> libraryById(const QUuid& uid);

    Account* loadAccount(const QUuid& uid);
    void loadLibraries();
    QVariantList accountUids();
    void saveAccountSecrets(Account* account);
    bool hasSecretsForAccount(Account* account);
    void setAccountSecret(const QUuid& accountUid, const QString& password);
    void initialize();
signals:
    void accountsChanged();
    void libraryLoaded(const QUuid& libraryUid);

private:
    QSettings* m_settings;
    QPointer<Cache> m_cache;
    QVariantMap m_secrets;
    QPointer<KeyStore> m_keyStore;
    QPointer<ProblemManager> m_problemManager;

    void importAccountsFromSynchronizers();
    void importAccountFromSynchronizer(const QString& syncUid, const QString& password);
};

#endif // DATASTORAGE_APPLICATIONSETTINGS_H_
