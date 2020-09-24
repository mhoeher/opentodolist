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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <QLoggingCategory>
#include <QObject>
#include <QQmlListProperty>
#include <QSettings>
#include <QStringList>
#include <QUrl>
#include <QVariantMap>
#include <QVector>

#include "datamodel/library.h"
#include "sync/synchronizer.h"
#include "utils/problem.h"
#include "utils/problemmanager.h"

class QRemoteObjectNode;
class QTemporaryDir;

class Account;
class ApplicationSettings;
class BackgroundServiceReplica;
class Cache;
class DirectoryWatcher;
class Image;
class KeyStore;
class Note;
class NotePage;
class Task;
class Todo;
class TodoList;

/**
 * @brief The main class of the application
 *
 * The Application class is used as entry point into the OpenTodoList application. It is used
 * as contained class and provides references to other objects. Basically, the Application class
 * models the application, i.e. it is created when the application starts and destroyed once
 * the application is to be closed.
 */
class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString librariesLocation READ librariesLocation CONSTANT)
    Q_PROPERTY(Cache* cache READ cache CONSTANT)
    Q_PROPERTY(QStringList directoriesWithRunningSync READ directoriesWithRunningSync NOTIFY
                       directoriesWithRunningSyncChanged)
    Q_PROPERTY(QVariantMap syncErrors READ syncErrors NOTIFY syncErrorsChanged)
    Q_PROPERTY(ProblemManager* problemManager READ problemManager CONSTANT)

public:
    explicit Application(Cache* cache, QObject* parent = nullptr);
    explicit Application(Cache* cache, const QString& applicationDir, QObject* parent = nullptr);

    virtual ~Application();

    Q_INVOKABLE void saveAccount(Account* account);
    Q_INVOKABLE void saveAccountSecrets(Account* account);
    Q_INVOKABLE void removeAccount(Account* account);
    Q_INVOKABLE Account* loadAccount(const QUuid& uid);
    Q_INVOKABLE QVariantList accountUids();

    Q_INVOKABLE Library* addLocalLibrary(const QString& name);
    Q_INVOKABLE Library* addLibraryDirectory(const QString& directory);
    Q_INVOKABLE Library* addNewLibraryToAccount(Account* account, const QString& name);
    Q_INVOKABLE Library* addExistingLibraryToAccount(Account* account,
                                                     const SynchronizerExistingLibrary& library);
    Q_INVOKABLE void deleteLibrary(Library* library);
    Q_INVOKABLE Note* addNote(Library* library, QVariantMap properties);
    Q_INVOKABLE NotePage* addNotePage(Library* library, Note* note, QVariantMap properties);
    Q_INVOKABLE Image* addImage(Library* library, QVariantMap properties);
    Q_INVOKABLE TodoList* addTodoList(Library* library, QVariantMap properties);
    Q_INVOKABLE Todo* addTodo(Library* library, TodoList* todoList, QVariantMap properties);
    Q_INVOKABLE Task* addTask(Library* library, Todo* todo, QVariantMap properties);
    Q_INVOKABLE void deleteItem(Item* item);
    Q_INVOKABLE void deleteDoneTodos(TodoList* todoList);
    Q_INVOKABLE void deleteDoneTasks(Todo* todo);

    Q_INVOKABLE void saveValue(const QString& name, const QVariant& value);
    Q_INVOKABLE QVariant loadValue(const QString& name, const QVariant& defaultValue = QVariant());

    Q_INVOKABLE QVariant find3rdPartyInfos() const;

    Q_INVOKABLE bool saveTextToFile(const QUrl& fileUrl, const QString& text) const;

    Q_INVOKABLE QString urlToLocalFile(const QUrl& url) const;
    Q_INVOKABLE QUrl localFileToUrl(const QString& localFile) const;
    Q_INVOKABLE QUrl cleanPath(const QUrl& url) const;
    Q_INVOKABLE QString htmlToPlainText(const QString& html) const;

    Q_INVOKABLE bool fileExists(const QString& filename) const;
    Q_INVOKABLE bool directoryExists(const QString& directory) const;
    Q_INVOKABLE QString basename(const QString& filename) const;
    Q_INVOKABLE bool isLibraryDir(const QUrl& url) const;
    Q_INVOKABLE QString libraryNameFromDir(const QUrl& url) const;

    Q_INVOKABLE QString sha256(const QString& text) const;

    QString librariesLocation() const;

    Q_INVOKABLE QUrl homeLocation() const;
    Q_INVOKABLE bool folderExists(const QUrl& url) const;

    Q_INVOKABLE bool libraryExists(const QUuid& uid);

    Cache* cache() const;

    QStringList directoriesWithRunningSync() const;
    void setDirectoriesWithRunningSync(const QStringList& directoriesWithRunningSync);

    QVariantMap syncErrors() const;

    ProblemManager* problemManager() const;

    void
    setPropagateCacheEventsFromBackgroundService(bool propagateCacheEventsFromBackgroundService);

public slots:

    void syncLibrary(Library* library);
    void copyToClipboard(const QString& text);
    void clearSyncErrors(Library* library);

signals:

    void directoriesWithRunningSyncChanged();
    void syncErrorsChanged();
    void accountsChanged();

    /**
     * @brief The user requested the application window to be shown.
     *
     * This signal is emitted to indicate that the user requested the main
     * window to be shown (again). For example, on Desktop systems, this
     * might be the case then the app runs minimized to the background and
     * the user clicks the tray icon - in this case, the Window shall
     * be show and brought to the foreground.
     */
    void showWindowRequested();

private:
    Cache* m_cache;
    KeyStore* m_keyStore;
    ProblemManager* m_problemManager;
    ApplicationSettings* m_appSettings;
    QSettings* m_settings;
    QStringList m_directoriesWithRunningSync;
    QVariantMap m_syncErrors;
    QSet<QString> m_librariesWithChanges;
    QSharedPointer<QTemporaryDir> m_tmpCacheDir;
    QRemoteObjectNode* m_remoteObjectNode;
    QSharedPointer<BackgroundServiceReplica> m_backgroundService;
    QSet<QUuid> m_librariesRequestedForDeletion;
    QUuid m_appInstanceUid;
    bool m_propagateCacheEventsFromBackgroundService;

    void initialize();

    void connectItemToCache(Item* item);

    void syncLibrariesWithCache(QList<QSharedPointer<Library>> libraries);

    template<typename T>
    void runSyncForLibrary(T library);

    template<typename T>
    void watchLibraryForChanges(T library);

    void internallyAddLibrary(Library* library);
    bool isLibraryUid(const QUuid& uid);

    void importAccountsFromSynchronizers();

    QSharedPointer<BackgroundServiceReplica> getBackgroundService();

private slots:

    void onLibrarySyncStarted(const QUuid& libraryUid);
    void onLibrarySyncFinished(const QUuid& libraryUid);
    void onLibrarySyncError(const QUuid& libraryUid, const QString& error);
    void onLibraryDeleted(const QUuid& libraryUid);
    void onLibrariesChanged(QVariantList librariesUids);
    void onBackgroundServiceCacheDataChanged(const QUuid& appInstanceUid);
    void onBackgroundServiceCacheLibrariesChanged(const QVariantList& libraryUids,
                                                  const QUuid& appInstanceUid);
    void onLocalCacheDataChanged();
    void onLocalCacheLibrariesChanged(const QVariantList& libraryUids);
};

#endif // APPLICATION_H_
