#ifndef LIBRARY_H
#define LIBRARY_H

#include <functional>

#include <QDir>
#include <QLoggingCategory>
#include <QObject>
#include <QString>
#include <QVariantMap>

#include "image.h"
#include "datastorage/itemcontainer.h"
#include "note.h"
#include "todolist.h"

class DirectoryWatcher;
class Application;
class Synchronizer;


/**
 * @brief Represents a library stored in the cache.
 */
struct LibraryCacheEntry {
    LibraryCacheEntry();
    LibraryCacheEntry(const LibraryCacheEntry &other) = default;
    LibraryCacheEntry& operator =(const LibraryCacheEntry &other) = default;
    bool operator ==(const LibraryCacheEntry &other) const;

    QByteArray toByteArray() const;
    static LibraryCacheEntry fromByteArray(const QByteArray &data, const QByteArray &id);


    QUuid id;
    QVariant data;
    QVariant metaData;
    bool valid;
};

Q_DECLARE_METATYPE(LibraryCacheEntry)




/**
 * @brief A container for items.
 *
 * The Library class is a container for TopLevelItem objects. Each library has a directory, for
 * which it loads and makes the contained items available.
 */
class Library : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUuid uid READ uid WRITE setUid NOTIFY uidChanged)
    Q_PROPERTY(bool isValid READ isValid CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QStringList tags READ tags NOTIFY tagsChanged)
    Q_PROPERTY(QString directory READ directory CONSTANT)
    Q_PROPERTY(ItemContainer* topLevelItems READ topLevelItems CONSTANT)
    Q_PROPERTY(ItemContainer* todos READ todos CONSTANT)
    Q_PROPERTY(ItemContainer* tasks READ tasks CONSTANT)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool hasSynchronizer READ hasSynchronizer CONSTANT)
    Q_PROPERTY(bool synchronizing READ synchronizing NOTIFY synchronizingChanged)
    Q_PROPERTY(bool secretsMissing READ secretsMissing WRITE setSecretsMissing
               NOTIFY secretsMissingChanged)
    Q_PROPERTY(QStringList syncErrors READ syncErrors NOTIFY syncErrorsChanged)
    Q_PROPERTY(bool isInDefaultLocation READ isInDefaultLocation CONSTANT)

    friend class Application;

public:

    static const QString LibraryFileName;

    explicit Library(QObject *parent = nullptr);
    explicit Library(const QString &directory, QObject* parent = nullptr);
    virtual ~Library();

    Q_INVOKABLE Note* addNote();
    Q_INVOKABLE Image* addImage();
    Q_INVOKABLE TodoList* addTodoList();

    bool isValid() const;

    LibraryCacheEntry encache() const;
    static Library* decache(const LibraryCacheEntry &entry,
                            QObject *parent = nullptr);
    static Library* decache(const QVariant &entry,
                            QObject *parent = nullptr);

    /**
     * @brief The name of the libary as used in the user interface.
     */
    QString name() const      { return m_name; }
    void setName( const QString &name );

    /**
     * @brief Returns the directory which the library encapsulates.
     */
    QString directory() const { return m_directory; }

    Q_INVOKABLE void deleteLibrary(bool deleteFiles = false);
    void deleteLibrary(bool deleteFiles, std::function<void ()> callback);
    Q_INVOKABLE bool load();
    Q_INVOKABLE bool save();
    Q_INVOKABLE QVariant syncLog();

    ItemContainer *topLevelItems();
    ItemContainer *todos();
    ItemContainer *tasks();

    QString newItemLocation() const;

    static QStringList years(const QString &directory);
    static QStringList months(const QString &directory, const QString &year);

    bool isInDefaultLocation() const;

    bool loading() const;
    QUuid uid() const;
    QStringList tags() const;

    void fromJson(const QByteArray data);

    bool hasSynchronizer() const;
    bool synchronizing() const;
    void setSynchronizing(bool synchronizing);

    Q_INVOKABLE Synchronizer *createSynchronizer(
            QObject *parent = nullptr) const;

    bool secretsMissing() const;
    void setSecretsMissing(bool secretsMissing);

    QStringList syncErrors() const;

signals:

    /**
     * @brief The UID of the library has changed.
     */
    void uidChanged();

    /**
     * @brief The name of the library changed.
     */
    void nameChanged();

    /**
     * @brief The list of tags changed.
     */
    void tagsChanged();

    /**
     * @brief A library is about to be deleted.
     *
     * This signal is emitted to indicate that the @p library is aboit to be
     * deleted. Clients might connect to this signal to do any additional clean
     * up required before the library data is removed.
     */
    void deletingLibrary(Library* library);

    /**
     * @brief The library is deleted.
     */
    void libraryDeleted(Library *library);

    void loadingChanged();

    /**
     * @brief Loading the items of the library finished.
     *
     * This signal is emitted to indicate that loading the items in a library
     * has finished.
     */
    void loadingFinished();

    void synchronizingChanged();

    void secretsMissingChanged();

    void syncErrorsChanged();

    /**
     * @brief The library needs to be synced.
     *
     * This signal is emitted to indicate that the library needs to be
     * synchronized.
     */
    void needSync();

public slots:

    void addSyncError(const QString &error);
    void clearSyncErrors();

private:

    QUuid                   m_uid;
    QString                 m_name;
    QString                 m_directory;

    ItemContainer           m_topLevelItems;
    ItemContainer           m_todos;
    ItemContainer           m_tasks;
    bool                    m_itemDataChanged;

    DirectoryWatcher       *m_directoryWatcher;

    bool                    m_loading;
    bool                    m_synchronizing;
    bool                    m_secretsMissing;
    QStringList             m_syncErrors;

    QVariantMap toMap() const;
    void fromMap(QVariantMap map);

    void setUid(const QUuid& uid);
    void setLoading(bool loading);

private slots:

    void appendItem(ItemPtr item);

};

#endif // LIBRARY_H
