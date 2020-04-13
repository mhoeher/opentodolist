#ifndef LIBRARYLOADER_H
#define LIBRARYLOADER_H

#include <QObject>
#include <QThread>
#include <QFutureWatcher>

#include "datamodel/item.h"

class LibraryLoaderWorker;
class Cache;
class InsertOrUpdateItemsQuery;

class LibraryLoader : public QObject
{
    Q_OBJECT
public:
    explicit LibraryLoader(QObject *parent = nullptr);
    virtual ~LibraryLoader();

    QString directory() const;
    void setDirectory(const QString &directory);

    QUuid libraryId() const;
    void setLibraryId(const QUuid &libraryId);

    Cache *cache() const;
    void setCache(Cache *cache);

signals:

    /**
     * @brief Scanning has finished.
     */
    void scanFinished();

public slots:

    void scan();

private:
    struct DirectoryScanResult
    {
        InsertOrUpdateItemsQuery *query;
        QSet<QUuid> itemsToDelete;
    };

    QUuid m_libraryId;
    QString m_directory;
    Cache *m_cache;
    QFutureWatcher<DirectoryScanResult> m_scanWatcher;
    QSet<QUuid> m_itemsToDelete;

private slots:

    void itemUidsLoaded(QSet<QUuid> uids);
    void directoryScanDone();
    void itemsInserted();
};

#endif // LIBRARYLOADER_H
