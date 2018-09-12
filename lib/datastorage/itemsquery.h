#ifndef ITEMSQUERY_H
#define ITEMSQUERY_H

#include <QObject>
#include <QSharedPointer>

class Cache;
class ItemsQueryRunnable;

// Forward declarations:
namespace QLMDB {
class Context;
class Database;
}

class ItemsQuery : public QObject
{
    Q_OBJECT
    friend class ItemsQueryRunnable;
    friend class Cache;
public:
    explicit ItemsQuery(QObject *parent = nullptr);
    ~ItemsQuery() override;

signals:

    /**
     * @brief This signal is emitted to indicate that the query is done.
     */
    void finished();

protected:
    QSharedPointer<QLMDB::Context> context() const;
    QSharedPointer<QLMDB::Database> global() const;
    QSharedPointer<QLMDB::Database> items() const;
    QSharedPointer<QLMDB::Database> children() const;

    /**
     * @brief Run the query.
     *
     * Sub-classes must implement this class to provide actual functionality.
     * Within their implementation, they can use methods like global(),
     * items() and children() to get the cache databases.
     */
    virtual void run() = 0;

private:

    QSharedPointer<QLMDB::Context> m_context;
    QSharedPointer<QLMDB::Database> m_global;
    QSharedPointer<QLMDB::Database> m_items;
    QSharedPointer<QLMDB::Database> m_children;

    void finish();
};

#endif // ITEMSQUERY_H
