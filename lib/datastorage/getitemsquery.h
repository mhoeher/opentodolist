#ifndef GETITEMSQUERY_H
#define GETITEMSQUERY_H

#include <QObject>

#include "datastorage/itemsquery.h"


class GetItemsQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit GetItemsQuery(QObject *parent = nullptr);

    QList<QUuid> parents() const;
    void setParents(const QList<QUuid> &parents);

    QUuid parent() const;
    void setParent(const QUuid &parent);

    bool recursive() const;
    void setRecursive(bool recursive);

signals:

    /**
     * @brief The items have been loaded.
     *
     * This signal is emitted to indicate that the requested items have
     * been loaded from the cache. The @p items list contains the cache
     * entries, which can be turned to item objects by calling Item::decache()
     * on each entry.
     */
    void itemsAvailable(QVariantList items);


    // ItemsQuery interface
protected:
    void run() override;

private:
    QList<QUuid> m_parents;
    bool m_recursive;
};

#endif // GETITEMSQUERY_H
