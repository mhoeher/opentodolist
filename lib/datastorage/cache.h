#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QSharedPointer>


// Forward declarations:
namespace QLMDB {
class Context;
class Database;
}


/**
 * @brief On-disk items cache.
 *
 * This class provides an on-disk cache for item data (excluding larger binary
 * data, like attached files). The advantage of having such a persistent cache
 * is fast startup time: The app can start showing the user the last known state
 * while in the background an update is running.
 *
 * This class provides a very easy to use interface to write items to and read
 * them from the cache.
 */
class Cache : public QObject
{
    Q_OBJECT
public:
    static const QByteArray RootId;
    static const QByteArray VersionKey;
    static const QByteArray Version_0;

    explicit Cache(QObject *parent = nullptr);
    virtual ~Cache();

    QString cacheDirectory() const;
    void setCacheDirectory(const QString &cacheDirectory);

    size_t cacheSize() const;
    void setCacheSize(size_t envSize);

    bool open();
    bool isValid() const;

signals:

public slots:

private:

    QSharedPointer<QLMDB::Context> m_context;
    QSharedPointer<QLMDB::Database> m_global;
    QSharedPointer<QLMDB::Database> m_items;
    QSharedPointer<QLMDB::Database> m_children;
    bool m_valid;

    bool openDBs();
    bool initV0();
};

#endif // CACHE_H
