#ifndef ITEMSQUERY_H
#define ITEMSQUERY_H

#include <QObject>
#include <QSharedPointer>
#include <QSet>

class Cache;
class ItemsQueryRunnable;

// Forward declarations:
namespace QLMDB {
class Context;
class Database;
class Transaction;
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

    /**
     * @brief Indicates that he query changed the cache contents.
     *
     * This signal is emitted once when the query is finished and if
     * the query changed the cache contents (by calling setDataChanged()).
     */
    void dataChanged();

    /**
     * @brief A list of UIDs of libraries which have been changed.
     *
     * This signal is emitted by some queries if they changed the database.
     * It contains a list of UIDs of the libraries which have been changed.
     */
    void librariesChanged(QVariantList libraries);

protected:
    QSharedPointer<QLMDB::Context> context() const;
    QSharedPointer<QLMDB::Database> global() const;
    QSharedPointer<QLMDB::Database> items() const;
    QSharedPointer<QLMDB::Database> children() const;
    void setDataChanged(bool changed = true);
    bool hasDataChanged() const;


    /**
     * @brief Run the query.
     *
     * Sub-classes must implement this class to provide actual functionality.
     * Within their implementation, they can use methods like global(),
     * items() and children() to get the cache databases.
     */
    virtual void run() = 0;

    virtual void markAsChanged(QLMDB::Transaction &transaction, QByteArray id);

private:

    QSharedPointer<QLMDB::Context> m_context;
    QSharedPointer<QLMDB::Database> m_global;
    QSharedPointer<QLMDB::Database> m_items;
    QSharedPointer<QLMDB::Database> m_children;
    bool m_dataChanged;
    QSet<QByteArray> m_changedLibrariesUids;
    QSet<QByteArray> m_changedParentUids;

    void finish();
};

#endif // ITEMSQUERY_H
