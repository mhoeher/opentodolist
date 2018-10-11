#ifndef LIBRARYLOADER_H
#define LIBRARYLOADER_H

#include <QObject>
#include <QThread>

#include "datamodel/item.h"

class LibraryLoaderWorker;
class Cache;

class LibraryLoader : public QObject
{
    Q_OBJECT
public:
    explicit LibraryLoader(QObject *parent = nullptr);
    virtual ~LibraryLoader();

    QString directory() const;
    void setDirectory(const QString& directory);

    QUuid libraryId() const;
    void setLibraryId(const QUuid &libraryId);

    Cache *cache() const;
    void setCache(Cache *cache);

signals:

    /**
     * @brief An item has been loaded.
     *
     * This signal is emitted to indicate that an @p item has been
     * found when scanning a library directory.
     */
    void itemLoaded(ItemPtr item);

    /**
     * @brief Scanning has finished.
     */
    void scanFinished();

public slots:

    void scan();

private:

    QUuid m_libraryId;
    QString m_directory;
    QThread m_thread;
    LibraryLoaderWorker *m_worker;
    Cache *m_cache;

};

class LibraryLoaderWorker : public QObject
{
    Q_OBJECT

    friend class LibraryLoader;

private:

    bool m_stop;

    LibraryLoaderWorker();

private slots:

    void scan(const QUuid &libraryId, const QString &directory, QObject* targetThread);

signals:

    void itemLoaded(ItemPtr item);
    void scanFinished();
};

#endif // LIBRARYLOADER_H
