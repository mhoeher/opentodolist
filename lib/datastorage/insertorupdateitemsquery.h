#ifndef INSERTORUPDATEITEMSQUERY_H
#define INSERTORUPDATEITEMSQUERY_H

#include <QObject>
#include <QUuid>
#include <QSet>

#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datastorage/itemsquery.h"

// Forward declaration:
class Item;

class InsertOrUpdateItemsQuery : public ItemsQuery
{
    Q_OBJECT
public:

    /**
     * @brief Run special instructions upon insertion/update.
     *
     * These flags can be used to run special instruction when the query is run.
     */
    enum InsertFlag {

        /**
         * @brief Just run the insertion/update.
         */
        NoFlags = 0,

        /**
         * @brief Calculate a new weight for the item.
         *
         * When this flag is set, a weight will be calculated for the item by
         * finding the maximum weight among all children of the same parent
         * item and adding some margin on top.
         *
         */
        CalcWeight = 0x01,

        /**
         * @brief Also save the item to disk.
         *
         * If this flag is set, also save the item to disk.
         */
        Save = 0x02,

        /**
         * @brief This is equivalent to `CalcWeight | Save`.
         */
        CreateNewItem = CalcWeight | Save
    };

    Q_DECLARE_FLAGS(InsertFlags, InsertFlag)

    explicit InsertOrUpdateItemsQuery(QObject *parent = nullptr);

public slots:

    void add(Item *item, InsertFlags flags = NoFlags);
    void add(Library* library);

protected:
    // ItemsQuery interface
    void run() override;

private:

    QList<ItemCacheEntry> m_itemEntries;
    QList<LibraryCacheEntry> m_libEntries;
    QSet<QUuid> m_calcWeight;
    QSet<QUuid> m_save;

};

#endif // INSERTORUPDATEITEMSQUERY_H
